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
#include <QOpenGLTexture>
#include <QImage>

class LiquidGlassWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit LiquidGlassWidget(QWidget *parent = nullptr);
    ~LiquidGlassWidget() override;

    /* Set the background image rendered behind the effect. */
    void setBackground(const QImage &image);

    /* Set the refraction intensity of the effect. */
    void setRefractionScale(float scale);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    QOpenGLShaderProgram m_program;
    GLuint m_vao {0};
    GLuint m_vbo {0};
    QOpenGLTexture *m_texture {nullptr};
    QImage m_background;
    float m_refractionScale {1.33f};
};

