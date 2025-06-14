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

LiquidGlassWidget::LiquidGlassWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

LiquidGlassWidget::~LiquidGlassWidget()
{
    makeCurrent();
    delete m_texture;
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
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
    m_refractionScale = scale;
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
    initializeOpenGLFunctions();

    QFile fragFile(":/shaders/liquidglass.frag");
    fragFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream fragStream(&fragFile);
    QString fragSrc = fragStream.readAll();

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc);
    m_program.link();

    static const float vertices[] = {
        -1.f, -1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f,
        -1.f, 1.f, 0.f, 1.f,
        1.f, 1.f, 1.f, 1.f
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
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void LiquidGlassWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_texture && !m_background.isNull()) {
        m_texture = new QOpenGLTexture(m_background);
        m_texture->setMinificationFilter(QOpenGLTexture::Linear);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    if (!m_texture)
        return;

    m_program.bind();
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();
    m_program.setUniformValue("sourceTexture", 0);
    m_program.setUniformValue("environmentTexture", 0);
    m_program.bind();

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    m_texture->release();
    m_program.release();
}

