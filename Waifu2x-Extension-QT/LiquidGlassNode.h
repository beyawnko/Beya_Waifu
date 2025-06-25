#pragma once

#include <QSGRenderNode>
#include <rhi/qrhi.h> // Main RHI header - Changed to rhi/qrhi.h
#include <rhi/qshader.h> // Added for QShader - Corrected include path for Qt 6
#include "RhiLiquidGlassItem.h" // For LiquidGlassParams struct

// Forward declarations
class QRhi;
class QRhiBuffer;
class QRhiTexture;
class QRhiSampler;
class QRhiGraphicsPipeline;
class QRhiShaderResourceBindings;
class QQuickWindow;

class LiquidGlassNode : public QSGRenderNode
{
public:
    LiquidGlassNode(QQuickWindow *window); // Pass window for RHI context
    ~LiquidGlassNode() override;

    void render(const RenderState *state) override;
    void preprocess() override; // For continuous updates like time

    // Called from RhiLiquidGlassItem::updatePaintNode to sync data
    void sync(RhiLiquidGlassItem *item);

private:
    void initializeRhiResources(); // Initialize RHI objects
    void releaseRhiResources();    // Release RHI objects
    void updateUniforms(RhiLiquidGlassItem *item); // Update UBO data
    void updateBackgroundTexture(const QImage &image); // Update background texture

    QQuickWindow *m_window; // Needed to get QRhi object
    QRhi *m_rhi = nullptr;  // Cache RHI object

    // RHI Resources
    QRhiBuffer *m_vertexBuffer = nullptr;
    QRhiBuffer *m_uniformBuffer = nullptr;
    QRhiTexture *m_backgroundTexture = nullptr;
    QRhiSampler *m_sampler = nullptr;
    QRhiShaderResourceBindings *m_srb = nullptr;
    QRhiGraphicsPipeline *m_pipeline = nullptr;

    // Shader stages (owned by QRhiGraphicsPipeline after creation, but good to hold refs during setup)
    QRhiShader m_vertexShader;
    QRhiShader m_fragmentShader;

    // Parameters
    LiquidGlassParams m_params;
    QImage m_currentBackground; // To detect changes
    bool m_resourcesInitialized = false;
    bool m_pipelineInitialized = false;

    // To manage updates from sync to render
    // These flags indicate if an update is needed in the render() call for QRhi resources
    bool m_uniformsDirty = true;
    bool m_backgroundTextureDirty = true;
    QRhiResourceUpdateBatch *m_resourceUpdates = nullptr; // For batching updates
};
