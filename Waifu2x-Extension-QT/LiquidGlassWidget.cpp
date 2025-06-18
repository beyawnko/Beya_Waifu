/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "LiquidGlassWidget.h"
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>
#include <QOpenGLContext>

LiquidGlassWidget::LiquidGlassWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);

    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &LiquidGlassWidget::updateElapsedTime);
    m_animationTimer->start(16);
}

LiquidGlassWidget::~LiquidGlassWidget()
{
    makeCurrent();
    m_ubo.destroy(); // Destroy UBO
    delete m_texture;
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);

    if (m_animationTimer && m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }
    doneCurrent();
}

void LiquidGlassWidget::setBackground(const QImage &image)
{
    m_background = image;
    if (isValid()) {
        makeCurrent();
        delete m_texture;
        m_texture = new QOpenGLTexture(m_background);
        m_texture->setMinificationFilter(QOpenGLTexture::Linear);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
        doneCurrent();
        update();
    }
}

void LiquidGlassWidget::setRefractionScale(float scale)
{
    m_refractionScale = scale; // This uniform is not in the UBO, kept as example if needed elsewhere
    update();
}

static const char *vertexSrc = R"(
    #version 330
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 texCoord;
    out vec2 vTexCoord;
    void main() {
        vTexCoord = texCoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

void LiquidGlassWidget::initializeGL()
{
    if (!initializeOpenGLFunctions()) {
        qWarning() << "Failed to initialize OpenGL functions";
        setEnabled(false);
        return;
    }

    QOpenGLContext *ctx = context();
    if (!ctx || ctx->format().majorVersion() < 3 ||
            (ctx->format().majorVersion() == 3 && ctx->format().minorVersion() < 3)) {
        qWarning() << "OpenGL 3.3 context required but" << (ctx ? QString::number(ctx->format().majorVersion()) + "." + QString::number(ctx->format().minorVersion()) : "no context");
        setEnabled(false);
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    QFile fragFile(":/shaders/liquidglass.frag");
    if (!fragFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open fragment shader file:" << fragFile.errorString();
        return;
    }
    QTextStream fragStream(&fragFile);
    QString fragSrc = fragStream.readAll();
    fragFile.close();

    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc)) {
        qWarning() << "Vertex shader compilation error:" << m_program.log();
        setEnabled(false);
        return;
    }
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc)) {
        qWarning() << "Fragment shader compilation error:" << m_program.log();
        setEnabled(false);
        return;
    }
    if (!m_program.link()) {
        qWarning() << "Shader program link error:" << m_program.log();
        setEnabled(false);
        return;
    }

    // Initialize UBO
    m_ubo.create();
    m_ubo.bind();
    m_ubo.allocate(nullptr, sizeof(LiquidGlassParams));
    m_ubo.release();

    m_program.bind();
    // Bind UBO block "SettingsBlock" to binding point 1
    GLuint blockIndex = glGetUniformBlockIndex(m_program.programId(), "SettingsBlock");
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(m_program.programId(), blockIndex, 1);
    } else {
        qWarning() << "Uniform block 'SettingsBlock' not found in shader.";
    }
    // Set sampler once
    m_program.setUniformValue("sourceTexture", 0);
    m_program.release();

    static const float vertices[] = {
        -1.f, -1.f, 0.f, 0.f,
         1.f, -1.f, 1.f, 0.f,
        -1.f,  1.f, 0.f, 1.f,
         1.f,  1.f, 1.f, 1.f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LiquidGlassWidget::resizeGL(int w, int h)
{
    m_widgetResolution = QVector2D(static_cast<float>(w), static_cast<float>(h));
    glViewport(0, 0, w, h);
}

void LiquidGlassWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_texture && !m_background.isNull()) {
        m_texture = new QOpenGLTexture(m_background);
        m_texture->setMinificationFilter(QOpenGLTexture::Linear);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    if (!m_texture) {
        return;
    }

    m_program.bind();

    // Populate UBO struct
    LiquidGlassParams params;
    params.resolution = m_widgetResolution;
    params.time = m_elapsedTime;
    // Adjust mouse y-coordinate: GL origin is bottom-left, QWidget top-left
    // Shader expects mouse coordinates relative to bottom-left of the widget.
    params.mouse = QVector2D(m_mousePosition.x(), m_widgetResolution.y() - m_mousePosition.y());
    params.ior = m_ior;
    params.borderRadius = m_borderRadius;
    params.chromaticAberrationOffset = m_chromaticAberrationOffset;
    // padding1 and padding2 are for layout and don't need to be set with specific values.

    // Upload data to UBO
    if (m_ubo.bind()) {
        m_ubo.write(0, &params, sizeof(LiquidGlassParams));
        m_ubo.release();
    }

    // Bind the UBO to the defined binding point for the current program state
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_ubo.bufferId());
    // The "sourceTexture" uniform (sampler2D) is set once in initializeGL.
    // No need to set it per frame unless the texture unit changes.

    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    m_texture->release();
    m_program.release();
}

// --- Setter Methods ---
void LiquidGlassWidget::setIOR(float ior)
{
    m_ior = ior;
    update();
}

void LiquidGlassWidget::setBorderRadius(float radius)
{
    m_borderRadius = radius;
    update();
}

void LiquidGlassWidget::setChromaticAberrationOffset(float offset)
{
    m_chromaticAberrationOffset = offset;
    update();
}

// --- Event Handlers and Slots ---
void LiquidGlassWidget::updateElapsedTime()
{
    m_elapsedTime += 0.016f;
    if (m_elapsedTime > 3600.0f) {
        m_elapsedTime = 0.0f;
    }
    update();
}

void LiquidGlassWidget::mouseMoveEvent(QMouseEvent *event)
{
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_mousePosition = event->position();
    #else
    m_mousePosition = event->localPos();
    #endif
    update();
}
