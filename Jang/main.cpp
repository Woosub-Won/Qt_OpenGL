#include <QGuiApplication>
#include <QOpenGLWindow>
#include "myopenglcore.h"

class CLASS_NAME : public QOpenGLWindow
{
protected:
    MyOpenGLCore *openglCore = nullptr;

#include <vector>
#include <QtMath>

    void initializeGL() override {
        // std::vector<GLfloat> vertices;   // 점 좌표
        // std::vector<GLfloat> normals;
        // std::vector<GLuint> indices;

        // // 결과 출력 (테스트용)
        // qDebug() << "Vertices: " << vertices.size() / 3 << " points\n";
        // qDebug() << "Normals: " << normals.size() / 3 << " points\n";
        // qDebug() << "Indices: " << indices.size() / 3 << " triangles\n";

        // openglCore = new MyOpenGLCore(vertices, indices, normals);

        QString objPath = "../../cube.obj";
        QString imagePath = "../../brick.jpg";
        openglCore = new MyOpenGLCore(objPath, imagePath);
        openglCore->initialize();

        qDebug() << "Using shader paths:" << "../../main.vert" << "../../main.frag";
        // 셰이더 파일 로드
        if (!openglCore->loadShaders("../../main.vert", "../../main.frag")) {
            qFatal("Failed to load shaders!");
        }
        openglCore->activeUniformVariables();

    }

    void paintGL() override {
        openglCore->render();
    }

    void keyPressEvent(QKeyEvent *event) override {
        openglCore->handleKeyPressEvent(event);
        update();
    }

    void mousePressEvent(QMouseEvent *event) override {
        openglCore->handleMousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        openglCore->handleMouseReleaseEvent(event); // 마우스 클릭 해제 처리
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        openglCore->handleMouseMoveEvent(event);
        update();
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QSurfaceFormat format;
    format.setVersion(4, 0); // OpenGL 4.0 Core Profile
    format.setProfile(QSurfaceFormat::CoreProfile);

    CLASS_NAME window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}





