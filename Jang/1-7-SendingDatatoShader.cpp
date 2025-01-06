#include <QMatrix4x4>
#include <QSurfaceFormat>
#include <QDebug>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "openglshaderwindow.h"

int one_seven_shader(int argc, char *argv[]) {
    // QApplication app(argc, argv);

    // qDebug() << "1-7";

    // // QSurfaceFormat 설정
    // QSurfaceFormat format;
    // format.setDepthBufferSize(24);
    // format.setStencilBufferSize(8);
    // format.setVersion(4, 0); // OpenGL 버전 3.3
    // format.setProfile(QSurfaceFormat::CoreProfile); // Core Profile
    // QSurfaceFormat::setDefaultFormat(format);

    // OpenGLShaderWindow window;
    // window.setFormat(format);
    // window.resize(640, 480);
    // window.show();

    // // Vertex 및 Color 데이터
    // const GLfloat vertices[] = {
    //     -0.5f, -0.5f,
    //     0.5f, -0.5f,
    //     0.0f,  0.5f
    // };

    // const GLfloat colors[] = {
    //     1.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f,
    //     0.0f, 0.0f, 1.0f
    // };

    // //window.initialize("../../tmp.vert", "../../tmp.frag", vertices, colors);
    // window.setAnimating(true);

    // return 0;

    QGuiApplication app(argc, argv);

    // Anti-Aliasing이나 기타 원하는 기능 설정
    QSurfaceFormat format;
    format.setSamples(16);
    // format.setDepthBufferSize(24);
    // format.setStencilBufferSize(8);
    format.setVersion(4, 0); // OpenGL 버전 3.3
    format.setProfile(QSurfaceFormat::CoreProfile); // Core Profile

    qDebug() << format;

    OpenGLShaderWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    // 애니메이션 활성화
    window.setAnimating(true);

    return app.exec();
}
