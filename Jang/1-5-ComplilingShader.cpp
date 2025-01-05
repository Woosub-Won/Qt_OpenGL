#include <QMatrix4x4>
#include <QSurfaceFormat>
#include <QDebug>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "openglshaderwindow.h"

int one_five_shader(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << "1-5";

    // // widget 같은걸 써야 opengl 연결 시작해서 있어야 돌아감
    // QOpenGLWidget *widget = new QOpenGLWidget();

    // widget->show();

    // QOpenGLFunctions glFunc;

    // glFunc.initializeOpenGLFunctions();

    // GLuint vertShader = glFunc.glCreateShader(GL_VERTEX_SHADER);
    // if (0 == vertShader){
    //     fprintf(stderr, "Error creating vertex shader\n");
    //     exit(1);
    // }

    // QOpenGLShaderProgram shaderProgram;
    // const char *shaderCode = shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, loadShaderSource("./1-5-basic.vert"));


    OpenGLShaderWindow window;
    window.setFormat(QSurfaceFormat::defaultFormat());
    window.resize(640, 480);
    //window.show();

    // 셰이더 파일 경로를 초기화 함수에 전달
    //window.initialize("../../tmp.vert", "../../tmp.frag");

    return 0;
    // return app.exec();
}
