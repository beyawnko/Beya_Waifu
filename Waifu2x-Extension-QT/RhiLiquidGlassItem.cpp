#include "RhiLiquidGlassItem.h"
#include "LiquidGlassNode.h" // The render node
#include <QQuickWindow>
#include <QSGNode>
#include <QMouseEvent>
#include <QHoverEvent> // For hover events

RhiLiquidGlassItem::RhiLiquidGlassItem(QQuickItem *parent)
    : QQuickItem(parent),
      m_animationTimer(new QTimer(this))
{
    setFlag(ItemHasContents, true); // This item has custom scene graph content
    setAcceptedMouseButtons(Qt::AllButtons); // Accept mouse events
    setAcceptHoverEvents(true); // Accept hover events for mouse tracking

    connect(m_animationTimer, &QTimer::timeout, this, &RhiLiquidGlassItem::onAnimationTimer);
    m_animationTimer->start(16); // ~60 FPS for animation updates
}

RhiLiquidGlassItem::~RhiLiquidGlassItem()
{
    if (m_animationTimer && m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }
    // QSGRenderNode (LiquidGlassNode) is managed by the scene graph if returned from updatePaintNode
}

QImage RhiLiquidGlassItem::background() const {
    return m_background;
}

void RhiLiquidGlassItem::setBackground(const QImage &image) {
    if (m_background != image) {
        m_background = image;
        m_backgroundDirty = true;
        emit backgroundChanged();
        update(); // Schedule an update to the scene graph
    }
}

float RhiLiquidGlassItem::ior() const {
    return m_ior;
}

void RhiLiquidGlassItem::setIor(float ior) {
    if (m_ior != ior) {
        m_ior = ior;
        emit iorChanged();
        update();
    }
}

float RhiLiquidGlassItem::borderRadius() const {
    return m_borderRadius;
}

void RhiLiquidGlassItem::setBorderRadius(float radius) {
    if (m_borderRadius != radius) {
        m_borderRadius = radius;
        emit borderRadiusChanged();
        update();
    }
}

float RhiLiquidGlassItem::chromaticAberrationOffset() const {
    return m_chromaticAberrationOffset;
}

void RhiLiquidGlassItem::setChromaticAberrationOffset(float offset) {
    if (m_chromaticAberrationOffset != offset) {
        m_chromaticAberrationOffset = offset;
        emit chromaticAberrationOffsetChanged();
        update();
    }
}

QSGNode *RhiLiquidGlassItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data); // updatePaintNodeData not used for now but could be for passing complex data

    LiquidGlassNode *node = static_cast<LiquidGlassNode *>(oldNode);

    if (!window()) { // Make sure we have a window
        if (node) {
            delete node; // Clean up if window is lost
        }
        return nullptr;
    }

    if (!node) {
        // Create a new node if one doesn't exist
        // The QQuickWindow is needed by the node to get RHI context
        node = new LiquidGlassNode(window());
    }

    // Synchronize data from this QQuickItem to the LiquidGlassNode
    // This is called on the render thread (or main thread if not threaded)
    // while the GUI thread is blocked, so direct access to item members is safe.
    node->sync(this);

    // Mark background as no longer dirty after sync has processed it
    // The node itself will handle its internal dirty state for texture updates
    m_backgroundDirty = false;

    return node;
}

void RhiLiquidGlassItem::mouseMoveEvent(QMouseEvent *event) {
    m_mousePosition = event->position();
    update(); // Schedule repaint
    QQuickItem::mouseMoveEvent(event);
}

void RhiLiquidGlassItem::hoverMoveEvent(QHoverEvent *event) {
    // QQuickItem needs acceptHoverEvents(true)
    m_mousePosition = event->position();
    update(); // Schedule repaint
    QQuickItem::hoverMoveEvent(event);
}


void RhiLiquidGlassItem::onAnimationTimer() {
    m_elapsedTime += 0.016f; // Approximate time since last frame
    if (m_elapsedTime > 3600.0f) { // Prevent overflow, reset periodically
        m_elapsedTime = 0.0f;
    }
    update(); // Schedule repaint to update time in shader
}

// Friend function or public getters needed by LiquidGlassNode::sync
// For now, LiquidGlassNode::sync will be a friend or RhiLiquidGlassItem will have getters
// For simplicity in this pass, making LiquidGlassNode a friend or adding getters.
// Let's assume LiquidGlassNode will be made a friend or use public getters.
// For now, we'll rely on LiquidGlassNode::sync being able to get data.
// In LiquidGlassNode::sync, we'd do:
// m_params.ior = item->ior();
// m_params.borderRadius = item->borderRadius();
// ... etc.
// if (item->isBackgroundDirty()) { updateBackgroundTexture(item->background()); }


#include "moc_RhiLiquidGlassItem.cpp"
