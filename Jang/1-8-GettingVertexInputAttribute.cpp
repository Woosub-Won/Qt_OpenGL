#include <QGuiApplication>
#include <QOpenGLWindow>
#include "myopenglcore.h"

class MyOpenGLWindow_one_eight : public QOpenGLWindow
{
protected:
    MyOpenGLCore *openglCore = nullptr;

    void initializeGL() override {
        std::vector<GLfloat> vertices = {
            0.0f,  0.5f, 0.0f,  // 위쪽 꼭짓점
            -0.5f, -0.5f, 0.0f,  // 왼쪽 아래
            0.5f, -0.5f, 0.0f   // 오른쪽 아래
        };

        std::vector<GLfloat> colors = {
            1.0f, 0.0f, 0.0f,  // 빨강
            0.0f, 1.0f, 0.0f,  // 초록
            0.0f, 0.0f, 1.0f   // 파랑
        };

        openglCore = new MyOpenGLCore(vertices, colors);
        openglCore->initialize();

        // 셰이더 파일 로드
        if (!openglCore->loadShaders("../../tmp.vert", "../../tmp.frag")) {
            qFatal("Failed to load shaders!");
        }
        // // 셰이더 파일 로드
        // if (!openglCore->loadShaders("vertex_shader.glsl", "fragment_shader.glsl")) {
        //     qFatal("Failed to load shaders!");
        // }

        // 1-8
        openglCore->activeVertexInputAttributesAndIndices();
    }

    void paintGL() override {
        openglCore->render();
    }
};

int main_one_eight(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 0); // OpenGL 4.0 Core Profile
    format.setProfile(QSurfaceFormat::CoreProfile);

    MyOpenGLWindow_one_eight window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}





