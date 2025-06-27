#include "LiquidGlassNode.h"
#include <QQuickWindow>
#include <QSGSimpleRectNode> // Example, not directly used for drawing here
#include <QFile>
#include <QTextStream>
#include <QDebug>

// RHI Includes - ensure these are sufficient and correct for the types used.
// QShader (for m_vertexShader, m_fragmentShader members) is in rhi/qshader.h (included via LiquidGlassNode.h)
// QRhi types (QRhiBuffer, QRhiTexture etc.) are in rhi/qrhi.h (included via LiquidGlassNode.h)
// QRhiCommandBuffer, QRhiGraphicsPipeline etc. are also from rhi/qrhi.h or related headers.

#include <QImage> // For texture creation
#include <QtQuick/qquickrendertarget.h> // For QQuickRenderTarget definition

// Define vertex data for a quad
static float quadVertices[] = {
    // position (x, y) texCoord (u, v)
    -1.0f, -1.0f,   0.0f, 0.0f,
     1.0f, -1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,   0.0f, 1.0f,
     1.0f,  1.0f,   1.0f, 1.0f
};


LiquidGlassNode::LiquidGlassNode(QQuickWindow *window)
    : m_window(window)
{
    // m_rhi will be initialized in initializeRhiResources when window()->rhi() is available
}

LiquidGlassNode::~LiquidGlassNode()
{
    releaseRhiResources();
}

void LiquidGlassNode::releaseRhiResources()
{
    if (m_pipeline) {
        m_pipeline->destroy();
        delete m_pipeline;
        m_pipeline = nullptr;
    }
    if (m_srb) {
        m_srb->destroy();
        delete m_srb;
        m_srb = nullptr;
    }
    if (m_vertexBuffer) {
        m_vertexBuffer->destroy();
        delete m_vertexBuffer;
        m_vertexBuffer = nullptr;
    }
    if (m_uniformBuffer) {
        m_uniformBuffer->destroy();
        delete m_uniformBuffer;
        m_uniformBuffer = nullptr;
    }
    if (m_backgroundTexture) {
        m_backgroundTexture->destroy();
        delete m_backgroundTexture;
        m_backgroundTexture = nullptr;
    }
    if (m_sampler) {
        m_sampler->destroy();
        delete m_sampler;
        m_sampler = nullptr;
    }
    if (m_resourceUpdates) { // If owns it
        delete m_resourceUpdates;
        m_resourceUpdates = nullptr;
    }
    m_vertexShader = {};
    m_fragmentShader = {};
    m_resourcesInitialized = false;
    m_pipelineInitialized = false;
}

// Moved definition earlier and changed return type to QShader
static QShader LiquidGlassNode_loadShader(const QString &resourcePath) {
    QFile f(resourcePath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load shader:" << resourcePath << f.errorString();
        return QShader(); // Return default-constructed QShader
    }
    // Assuming QRhiShader::fromSerialized was a typo and it should be QShader::fromSerialized
    // Or, if loading raw SPIR-V, QShader has other ways to be initialized.
    // For .qsb files (Qt Shader Bakery), QShader::fromSerialized is correct.
    return QShader::fromSerialized(f.readAll());
}


void LiquidGlassNode::initializeRhiResources()
{
    if (!m_window || !m_window->rhi()) {
        qWarning("LiquidGlassNode: No RHI available from window.");
        return;
    }
    m_rhi = m_window->rhi();
    if (!m_rhi) {
         qWarning("LiquidGlassNode: RHI object is null after getting from window.");
        return;
    }


    // Vertex Buffer
    m_vertexBuffer = m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(quadVertices));
    m_vertexBuffer->create();

    // Uniform Buffer
    m_uniformBuffer = m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(LiquidGlassParams));
    m_uniformBuffer->create();

    // Sampler for the texture
    m_sampler = m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None,
                                   QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge);
    m_sampler->create();

    // Shader Resource Bindings (describes UBOs, textures, samplers)
    m_srb = m_rhi->newShaderResourceBindings();
    m_srb->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, m_uniformBuffer),
        QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, nullptr, m_sampler) // Texture will be set later
    });
    m_srb->create();

    // Load Shaders (assuming they are in resources compiled by qsb)
    // Note: Path to shaders might need adjustment based on .qrc prefix and SHADERS variable in .pro
    m_vertexShader = LiquidGlassNode_loadShader(QStringLiteral(":/shaders/liquidglass.vert.qsb"));
    m_fragmentShader = LiquidGlassNode_loadShader(QStringLiteral(":/shaders/liquidglass.frag.qsb"));

    if (!m_vertexShader.isValid() || !m_fragmentShader.isValid()) {
        qWarning("LiquidGlassNode: Failed to load shaders. Rendering will not work.");
        releaseRhiResources(); // Clean up partially initialized resources
        return;
    }

    m_resourceUpdates = m_rhi->nextResourceUpdateBatch();
    m_resourceUpdates->uploadStaticBuffer(m_vertexBuffer, quadVertices);
    // Initial UBO data can be uploaded here or in first sync/render
    // Initial texture can be created here if a default background exists

    m_resourcesInitialized = true;
    m_uniformsDirty = true; // Force initial UBO update
    m_backgroundTextureDirty = true; // Force initial texture update if background is set
}

void LiquidGlassNode::updateBackgroundTexture(const QImage &image) {
    if (!m_rhi || image.isNull()) {
        if (m_backgroundTexture) { // If image becomes null, release existing texture
            m_backgroundTexture->destroy();
            delete m_backgroundTexture;
            m_backgroundTexture = nullptr;
            // Update SRB to reflect no texture or a default one
            if (m_srb && m_sampler) { // Ensure sampler is also valid
                 m_srb->setBindings({
                    QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, m_uniformBuffer),
                    QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, nullptr, m_sampler)
                });
            }
        }
        return;
    }

    if (m_backgroundTexture) {
        // Check if size or format changed, might need to recreate
        if (m_backgroundTexture->pixelSize() != image.size() || m_backgroundTexture->format() != QRhiTexture::RGBA8) { // Assuming RGBA8 for simplicity
            m_backgroundTexture->destroy();
            delete m_backgroundTexture;
            m_backgroundTexture = nullptr;
        }
    }

    if (!m_backgroundTexture) {
        // Ensure image is in a supported format for QRhiTexture::createFromImage, typically RGBA8 or BGRA8
        QImage compatibleImage = image.convertToFormat(QImage::Format_RGBA8888);
        if (compatibleImage.isNull()) {
            qWarning("LiquidGlassNode: Failed to convert background image to RGBA8888");
            return;
        }

        m_backgroundTexture = m_rhi->newTexture(QRhiTexture::RGBA8, compatibleImage.size(), 1, QRhiTexture::Flags(QRhiTexture::Flag::DontGenerateMips)); // Corrected flag usage
        if (!m_backgroundTexture) {
            qWarning("LiquidGlassNode: Failed to create RHI texture object.");
            return;
        }
        m_backgroundTexture->create();
        // Initial data upload
        if (!m_resourceUpdates) m_resourceUpdates = m_rhi->nextResourceUpdateBatch();
        m_resourceUpdates->uploadTexture(m_backgroundTexture, compatibleImage);

    } else {
        // Update existing texture data if necessary (e.g. if image content changed but size/format is same)
        // This might be more complex if only part of the texture updates. For full replace:
        QImage compatibleImage = image.convertToFormat(QImage::Format_RGBA8888);
        if (compatibleImage.isNull()) return;
        if (!m_resourceUpdates) m_resourceUpdates = m_rhi->nextResourceUpdateBatch();
        m_resourceUpdates->uploadTexture(m_backgroundTexture, compatibleImage);
    }

    // Update SRB with the new texture
    if (m_srb) {
        m_srb->setBindings({
            QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, m_uniformBuffer),
            QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, m_backgroundTexture, m_sampler)
        });
    }
    m_currentBackground = image; // Store the image that the current texture represents
}


void LiquidGlassNode::updateUniforms(RhiLiquidGlassItem *item) {
    if (!m_uniformBuffer || !item) return;

    m_params.resolution = QVector2D(item->width(), item->height());
    m_params.time = item->elapsedTime();

    // Adjust mouse Y: QQuickItem mouse is top-left, shader expects bottom-left
    QPointF qmlMousePos = item->mousePosition();
    m_params.mouse = QVector2D(qmlMousePos.x(), item->height() - qmlMousePos.y());

    m_params.ior = item->ior();
    m_params.borderRadius = item->borderRadius();
    m_params.chromaticAberrationOffset = item->chromaticAberrationOffset();

    if (!m_resourceUpdates) m_resourceUpdates = m_rhi->nextResourceUpdateBatch();
    m_resourceUpdates->updateDynamicBuffer(m_uniformBuffer, 0, sizeof(LiquidGlassParams), &m_params);
}


void LiquidGlassNode::sync(RhiLiquidGlassItem *item) {
    if (!item) return;

    if (!m_resourcesInitialized) {
        initializeRhiResources();
        if (!m_resourcesInitialized) return; // Initialization failed
    }

    // Check if background image has changed
    if (item->isBackgroundDirty() || (m_backgroundTexture && m_currentBackground != item->background()) || (!m_backgroundTexture && !item->background().isNull())) {
        updateBackgroundTexture(item->background());
        // m_backgroundTextureDirty is an internal flag of the node, RhiLiquidGlassItem's m_backgroundDirty is reset in its updatePaintNode
    }

    // Always update uniforms if values could have changed.
    // More fine-grained dirty tracking could be added.
    updateUniforms(item);
    m_uniformsDirty = false; // Mark as processed for this sync

    // If pipeline is not created, or depends on something that changed (e.g. render target format from window)
    // For this example, we assume render target format is stable enough once window is set.
    // Pipeline needs to be created after SRB and shaders are ready.
    // Use renderTarget() from QSGRenderNode, which returns QRhiRenderTarget*
    const QRhiRenderTarget *rt = renderTarget();
    if (!m_pipelineInitialized && m_resourcesInitialized && rt) { // rt is a pointer, check directly
        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({
            { 4 * sizeof(float) } // Stride: vec2 pos, vec2 texCoord
        });
        inputLayout.setAttributes({
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },                    // Position
            { 0, 1, QRhiVertexInputAttribute::Float2, 2 * sizeof(float) }     // TexCoord
        });

        m_pipeline = m_rhi->newGraphicsPipeline();
        m_pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
        // Set any blending, depth test, stencil, cull mode etc. here if needed
        // For default opaque drawing:
        m_pipeline->setSampleCount(rt->sampleCount()); // Use rt->

        // Example blend for transparency (if needed, adapt as per original GL widget)
        // QRhiGraphicsPipeline::TargetBlend blendState;
        // blendState.enable = true;
        // blendState.srcColor = QRhiGraphicsPipeline::SrcAlpha;
        // blendState.dstColor = QRhiGraphicsPipeline::OneMinusSrcAlpha;
        // m_pipeline->setTargetBlends({blendState});


        m_pipeline->setShaderStages({
            { QRhiShaderStage::Vertex, m_vertexShader },
            { QRhiShaderStage::Fragment, m_fragmentShader }
        });
        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb);
        m_pipeline->setRenderPassDescriptor(rt->renderPassDescriptor()); // Use rt->

        if (!m_pipeline->create()) {
            qWarning("LiquidGlassNode: Failed to create graphics pipeline.");
            releaseRhiResources(); // Clean up
            return;
        }
        m_pipelineInitialized = true;
    }
}

void LiquidGlassNode::render(const QSGRenderNode::RenderState *state) { // Added QSGRenderNode::
    if (!m_pipelineInitialized || !m_rhi || !m_pipeline || !state) {
        return;
    }

    QRhiCommandBuffer *cb = commandBuffer(); // Use QSGRenderNode::commandBuffer()
    if (!cb) return;

    // Process any pending resource updates (textures, UBO, VBO)
    if (m_resourceUpdates) {
        cb->resourceUpdate(m_resourceUpdates); // Submit the batch
        delete m_resourceUpdates; // QRhi takes ownership, but best practice to nullify
        m_resourceUpdates = nullptr;
    }

    // If no background texture, don't render (or render clear color)
    if (!m_backgroundTexture && !m_params.resolution.isNull()){ // only skip if we expect a texture
         // Option: could fill with a solid color or just not draw
         // For now, if no texture, we just won't draw the effect.
         // The QML item background itself might show through.
        return;
    }

    // Use renderTarget() for RHI properties like pixelSize
    const QRhiRenderTarget *rt = renderTarget();
    if (!rt) return; // Should not happen if pipeline is initialized
    const QSize outputPixelSize = rt->pixelSize(); // Use rt->pixelSize()
    cb->setViewport(QRhiViewport(0, 0, outputPixelSize.width(), outputPixelSize.height()));
    cb->setGraphicsPipeline(m_pipeline);
    cb->setShaderResources(m_srb); // Bind UBO and texture sampler

    const QRhiCommandBuffer::VertexInput vtxInput(m_vertexBuffer, 0);
    cb->setVertexInput(0, 1, &vtxInput);
    cb->draw(4); // Draw the quad (4 vertices, TriangleStrip)
}

void LiquidGlassNode::preprocess() {
    // This function is called on the render thread before render()
    // It's a good place for continuous updates or checks.
    // For time-based animation, if RhiLiquidGlassItem::onAnimationTimer calls update(),
    // then sync() will be called, which updates the UBO.
    // If not, preprocess() could markDirty(QSGNode::DirtyMaterial) to ensure render() is called
    // and uniforms are fresh.
    // For now, assuming item->update() handles triggering repaint.
    // If not, we'd need:
    // if (m_resourcesInitialized) {
    //     markDirty(DirtyMaterial); // Or a more specific flag if available for uniform changes
    // }
}
}
