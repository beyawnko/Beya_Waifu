#pragma once
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

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core> // Use a specific version
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer> // Added for UBO
#include <QOpenGLTexture>
#include <QImage>
#include <QVector2D>
#include <QPointF>
#include <QTimer>
#include <QMouseEvent>
#include "LiquidGlassParams.h" // Include the new header

class LiquidGlassWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit LiquidGlassWidget(QWidget *parent = nullptr);
    ~LiquidGlassWidget() override;

    /* Set the background image rendered behind the effect. */
    void setBackground(const QImage &image);

    /* Set the refraction intensity of the effect. */
    void setRefractionScale(float scale); // Existing setter

public slots:
    void setIOR(float ior);
    void setBorderRadius(float radius);
    void setChromaticAberrationOffset(float offset);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void updateElapsedTime();

private:
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_ubo; // UBO for shader parameters
    GLuint m_vao {0};
    GLuint m_vbo {0};
    QOpenGLTexture *m_texture {nullptr};
    QImage m_background;
    float m_refractionScale {1.33f};

    // Members for shader parameters (will be loaded into UBO)
    float m_ior = 1.5f;
    float m_borderRadius = 15.0f;
    float m_chromaticAberrationOffset = 0.005f;
    QVector2D m_widgetResolution;
    QPointF m_mousePosition;
    float m_elapsedTime = 0.0f;

    // Timer for animation
    QTimer *m_animationTimer;
};
