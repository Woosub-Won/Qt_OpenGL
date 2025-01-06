#ifndef QT_OPENGL_WINDOW_EXAMPLE_TRIANGLEWINDOW_H
#define QT_OPENGL_WINDOW_EXAMPLE_TRIANGLEWINDOW_H

#include "qt_opengl_window_example_openglwindow.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QScreen>

static const char *vertexShaderSource = "attribute highp vec4 posAttr;\n"
                                        "attribute lowp vec4 colAttr;\n"
                                        "varying lowp vec4 col;\n"
                                        "uniform highp mat4 matrix;\n"
                                        "void main() {\n"
                                        "   col = colAttr;\n"
                                        "   gl_Position = matrix * posAttr;\n"
                                        "}\n";

static const char *fragmentShaderSource = "varying lowp vec4 col;\n"
                                          "void main() {\n"
                                          "   gl_FragColor = col;\n"
                                          "}\n";

class TriangleWindow : public OpenGLWindow
{
public:
    using OpenGLWindow::OpenGLWindow;

    void initialize() override;
    void render() override;

private:
    GLint m_matrixUniform = 0;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram *m_program = nullptr;
    int m_frame = 0;
};

#endif // QT_OPENGL_WINDOW_EXAMPLE_TRIANGLEWINDOW_H
