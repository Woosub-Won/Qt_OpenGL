#include <QGuiApplication>
#include <QOpenGLWindow>
#include <QSurfaceFormat>
#include <QDebug>
#include "myopenglcore.h"

class MyWindow : public QOpenGLWindow
{
public:
    MyWindow() : openglCore(nullptr) {}

protected:
    void initializeGL() override {
        // QOpenGLWindow과 관련된 GL context 세팅 이후 호출
        openglCore = new MyOpenGLCore(
            "../../cube.obj",       // 원하는 obj 파일 경로
            "../../brick.jpg"       // 원하는 텍스처 이미지 경로
            );
        // 이제 셰이더 로드 및 각종 초기화
        openglCore->initialize("../../main.vert", "../../main.frag");
        // 디버깅용: Uniform/Attrib 확인
        openglCore->activeUniformVariables();
        openglCore->activeVertexInputAttributesAndIndices();
    }

    void paintGL() override {
        if(openglCore) {
            openglCore->render();
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        if(openglCore) {
            openglCore->handleKeyPressEvent(event);
        }
        update(); // 새 프레임 그려달라고 요청
    }

    void mousePressEvent(QMouseEvent *event) override {
        if(openglCore) {
            openglCore->handleMousePressEvent(event);
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if(openglCore) {
            openglCore->handleMouseReleaseEvent(event);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if(openglCore) {
            openglCore->handleMouseMoveEvent(event);
        }
        update(); // 새 프레임 그려달라고 요청
    }

private:
    MyOpenGLCore *openglCore;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 원하는 OpenGL 버전 세팅
    QSurfaceFormat format;
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);

    MyWindow window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}
