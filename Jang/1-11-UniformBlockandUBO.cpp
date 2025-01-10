#include <QGuiApplication>
#include <QOpenGLWindow>
#include "myopenglcore.h"

// Define으로 변경될 이름과 경로 설정
#define _CHAPTER 1-11-UniformBlockandUBO
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
        const float radius = 0.5f;  // 반지름
        const int numPoints = 6;    // 꼭짓점 개수
        std::vector<GLfloat> vertices;   // 점 좌표
        std::vector<GLfloat> texCoords; // 텍스처 좌표
        std::vector<GLfloat> colors = {
            1.0f, 0.0f, 0.0f,  // 빨강
            0.0f, 1.0f, 0.0f,  // 초록
            0.0f, 1.0f, 1.0f   // 파랑
        };

        // 점과 텍스처 좌표 생성
        for (int i = 0; i < numPoints; ++i) {
            float angle = 2.0f * M_PI * i / numPoints;  // 각도 (라디안 단위)
            float x = radius * cos(angle);             // 점 좌표 x
            float y = radius * sin(angle);             // 점 좌표 y
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);                  // z축 (2D이므로 0)

            // 텍스처 좌표 계산: [0, 1] 범위로 변환
            float u = x * 0.5f + 0.5f;
            float v = y * 0.5f + 0.5f;
            texCoords.push_back(u);
            texCoords.push_back(v);
        }

        // QDebug로 점 좌표와 텍스처 좌표 출력
        qDebug() << "점 번호 | 좌표 (x, y, z)       | 텍스처 좌표 (u, v)";
        qDebug() << "--------------------------------------------------";
        for (int i = 0; i < numPoints; ++i) {
            qDebug().noquote() << QString::asprintf(
                " %-6d | (%.2f, %.2f, %.2f) | (%.2f, %.2f) | (%.2f, %.2f) | (%.2f, %.2f)",
                i,                        // 점 번호
                vertices[i * 3],          // x
                vertices[i * 3 + 1],      // y
                vertices[i * 3 + 2],      // z
                texCoords[i * 2],         // u
                texCoords[i * 2 + 1],      // v
                (texCoords[i * 2]-0.5) * (texCoords[i * 2]-0.5),         // u
                (texCoords[i * 2 + 1]-0.5) * (texCoords[i * 2 + 1]-0.5),      // v
                (texCoords[i * 2]-0.5) * (texCoords[i * 2]-0.5) + (texCoords[i * 2 + 1]-0.5) * (texCoords[i * 2 + 1]-0.5),         // u
                sqrt((texCoords[i * 2 + 1]-0.5) * (texCoords[i * 2 + 1]-0.5) + (texCoords[i * 2 + 1]-0.5) * (texCoords[i * 2 + 1]-0.5))      // v
                );
        }

        openglCore = new MyOpenGLCore(vertices, colors, texCoords);
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

int main(int argc, char *argv[])
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





