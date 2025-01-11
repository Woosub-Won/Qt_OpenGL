#include <QGuiApplication>
#include <QOpenGLWindow>
#include "myopenglcore.h"

// Define으로 변경될 이름과 경로 설정
#define _CHAPTER 1-9-UniformVariables
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// 클래스 이름과 셰이더 경로 생성
#define CLASS_NAME MyOpenGLWindow_##_CHAPTER
#define DONT_COMPILE_CLASS_NAME THIS_FOR_NOT_USING_IF_USE_CHANGE_TO_CLASS_NAME_##_CHAPTER
#define VERTEX_SHADER_PATH TO_STRING(../../_CHAPTER.vert)
#define FRAGMENT_SHADER_PATH TO_STRING(../../_CHAPTER.frag)

class DONT_COMPILE_CLASS_NAME : public QOpenGLWindow
{
protected:
    MyOpenGLCore *openglCore = nullptr;

    void initializeGL() override {
        std::vector<GLfloat> vertices = {
            0.5f,  0.5f, 0.0f,  // 위쪽 꼭짓점
            -0.5f, -0.5f, 0.0f,  // 왼쪽 아래
            0.5f, -0.5f, 0.0f   // 오른쪽 아래
        };

        std::vector<GLfloat> colors = {
            1.0f, 0.0f, 0.0f,  // 빨강
            0.0f, 1.0f, 0.0f,  // 초록
            0.0f, 1.0f, 1.0f   // 파랑
        };

        openglCore = new MyOpenGLCore(vertices, colors);
        openglCore->initialize();

        qDebug() << "Using shader paths:" << VERTEX_SHADER_PATH << FRAGMENT_SHADER_PATH;
        // 셰이더 파일 로드
        if (!openglCore->loadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH)) {
            qFatal("Failed to load shaders!");
        }

    }

    void paintGL() override {
        openglCore->render();
    }
};

int main_one_nine(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QSurfaceFormat format;
    format.setVersion(4, 0); // OpenGL 4.0 Core Profile
    format.setProfile(QSurfaceFormat::CoreProfile);

    DONT_COMPILE_CLASS_NAME window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}





