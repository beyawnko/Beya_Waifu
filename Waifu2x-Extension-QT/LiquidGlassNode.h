#pragma once

#include <QSGRenderNode>
// Includes for RHI, using QShader
#include <rhi/qrhi.h>    // For QRhi, QRhiBuffer, QRhiShaderStage, etc.
#include <rhi/qshader.h> // For QShader

#include "RhiLiquidGlassItem.h" // For LiquidGlassParams struct

// Forward declarations for other types
class QQuickWindow;
// Other QRhi types are defined in rhi/qrhi.h or rhi/qshader.h

class LiquidGlassNode : public QSGRenderNode
{
public:
    LiquidGlassNode(QQuickWindow *window);
    ~LiquidGlassNode() override;

    void render(const RenderState *state) override;
    void preprocess() override;

    void sync(RhiLiquidGlassItem *item);

private:
    void initializeRhiResources();
    void releaseRhiResources();
    void updateUniforms(RhiLiquidGlassItem *item);
    void updateBackgroundTexture(const QImage &image);

    QQuickWindow *m_window;
    QRhi *m_rhi = nullptr;

    QRhiBuffer *m_vertexBuffer = nullptr;
    QRhiBuffer *m_uniformBuffer = nullptr;
    QRhiTexture *m_backgroundTexture = nullptr;
    QRhiSampler *m_sampler = nullptr;
    QRhiShaderResourceBindings *m_srb = nullptr;
    QRhiGraphicsPipeline *m_pipeline = nullptr;

    QShader m_vertexShader;   // Corrected type to QShader
    QShader m_fragmentShader; // Corrected type to QShader

    LiquidGlassParams m_params; // Requires RhiLiquidGlassItem.h
    QImage m_currentBackground;
    bool m_resourcesInitialized = false;
    bool m_pipelineInitialized = false;

    bool m_uniformsDirty = true;
    bool m_backgroundTextureDirty = true;
    QRhiResourceUpdateBatch *m_resourceUpdates = nullptr;
};
