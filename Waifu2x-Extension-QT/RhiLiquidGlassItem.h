#pragma once

#include <QQuickItem>
#include <QImage>
#include <QTimer>
#include <QBasicTimer> // For m_elapsedTime basic timer
#include "LiquidGlassParams.h" // Include the new header

// Forward declaration
class LiquidGlassNode;

class RhiLiquidGlassItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(float ior READ ior WRITE setIor NOTIFY iorChanged)
    Q_PROPERTY(float borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)
    Q_PROPERTY(float chromaticAberrationOffset READ chromaticAberrationOffset WRITE setChromaticAberrationOffset NOTIFY chromaticAberrationOffsetChanged)
    // Q_PROPERTY(QRectF sourceRect READ sourceRect WRITE setSourceRect NOTIFY sourceRectChanged) // If we want to support source rect for texture

public:
    RhiLiquidGlassItem(QQuickItem *parent = nullptr);
    ~RhiLiquidGlassItem() override;

    QImage background() const;
    void setBackground(const QImage &image);

    float ior() const;
    void setIor(float ior);

    float borderRadius() const;
    void setBorderRadius(float radius);

    float chromaticAberrationOffset() const;
    void setChromaticAberrationOffset(float offset);

    // Getters for LiquidGlassNode
    QPointF mousePosition() const { return m_mousePosition; }
    float elapsedTime() const { return m_elapsedTime; }
    bool isBackgroundDirty() const { return m_backgroundDirty; }


signals:
    void backgroundChanged();
    void iorChanged();
    void borderRadiusChanged();
    void chromaticAberrationOffsetChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override; // Also handle hover for mouse tracking

private slots:
    void onAnimationTimer(); // Slot for QTimer

private:
    QImage m_background;
    bool m_backgroundDirty = true;

    float m_ior = 1.5f;
    float m_borderRadius = 15.0f;
    float m_chromaticAberrationOffset = 0.005f;

    QPointF m_mousePosition; // Store physical mouse position
    float m_elapsedTime = 0.0f;
    QTimer *m_animationTimer; // For smooth animation updates
};
