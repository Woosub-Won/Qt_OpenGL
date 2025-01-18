#include <QGuiApplication>
#include <QOpenGLWindow>
#include "myopenglcore.h"

class CLASS_NAME : public QOpenGLWindow
{
protected:
    MyOpenGLCore *openglCore = nullptr;

#include <vector>
#include <QtMath>

    void generateTorus(
        std::vector<GLfloat>& vertices,
        std::vector<GLfloat>& normals,
        float radius, float tubeRadius,
        int radialSegments, int tubularSegments)
    {
        // 0 <= i < radialSegments, 0 <= j < tubularSegments
        // (실제로는 +1 해서 중복 정점을 둔 뒤에 인덱스에서 wrap 해주는 경우도 많음)
        for (int i = 0; i < radialSegments; i++) {
            float theta = 2.0f * M_PI * (float)i / (float)radialSegments;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            for (int j = 0; j < tubularSegments; j++) {
                float phi = 2.0f * M_PI * (float)j / (float)tubularSegments;
                float cosPhi = std::cos(phi);
                float sinPhi = std::sin(phi);

                // 큰 원 중심
                float cx = radius * cosTheta;
                float cy = radius * sinTheta;

                // 정점 좌표
                float x = (radius + tubeRadius * cosPhi) * cosTheta;
                float y = (radius + tubeRadius * cosPhi) * sinTheta;
                float z = tubeRadius * sinPhi;

                // 법선 벡터
                float nx = cosPhi * cosTheta;
                float ny = cosPhi * sinTheta;
                float nz = sinPhi;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }
    }

    // (tubularSegments + 1) 등으로 정점을 저장했다면,
    // wrapIndex(i+1) 같은 식으로 모듈로 연산을 해줘도 된다.
    inline int idx(int i, int j, int tubularSegments) {
        return i * tubularSegments + j;
    }

    void generateTorusIndices(
        std::vector<GLuint>& indices,
        int radialSegments, int tubularSegments)
    {
        // i, j를 0부터 시작해 radialSegments, tubularSegments를 넘어가지 않도록
        // 그리고 (i+1, j+1)에 대해서는 wrap(모듈로) 처리
        for (int i = 0; i < radialSegments; i++) {
            for (int j = 0; j < tubularSegments; j++) {
                // 다음 i, j (wrap)
                int iNext = (i + 1) % radialSegments;
                int jNext = (j + 1) % tubularSegments;

                // 격자 한 칸에 해당하는 정점 인덱스
                GLuint a = idx(i,     j,     tubularSegments);
                GLuint b = idx(iNext, j,     tubularSegments);
                GLuint c = idx(i,     jNext, tubularSegments);
                GLuint d = idx(iNext, jNext, tubularSegments);

                // 첫 번째 삼각형
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                // 두 번째 삼각형
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }


    // // 도넛 정점 생성 함수
    // void generateTorus(std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, float radius, float tubeRadius, int radialSegments, int tubularSegments) {
    //     // 도넛 생성
    //     for (int i = 0; i <= radialSegments; ++i) {
    //         float theta = 2.0 * M_PI * i / radialSegments; // 큰 원의 각도
    //         float cosTheta = qCos(theta);
    //         float sinTheta = qSin(theta);

    //         for (int j = 0; j <= tubularSegments; ++j) {
    //             float phi = 2.0 * M_PI * j / tubularSegments; // 작은 원의 각도
    //             float cosPhi = qCos(phi);
    //             float sinPhi = qSin(phi);

    //             // 큰 원의 중심 좌표
    //             float cx = radius * cosTheta;
    //             float cy = radius * sinTheta;

    //             // 정점 좌표 계산
    //             float x = (radius + tubeRadius * cosPhi) * cosTheta;
    //             float y = (radius + tubeRadius * cosPhi) * sinTheta;
    //             float z = tubeRadius * sinPhi;

    //             // 법선 벡터 계산
    //             float nx = cosPhi * cosTheta;
    //             float ny = cosPhi * sinTheta;
    //             float nz = sinPhi;

    //             // 정점 추가
    //             vertices.push_back(x);
    //             vertices.push_back(y);
    //             vertices.push_back(z);

    //             // 법선 추가
    //             normals.push_back(nx);
    //             normals.push_back(ny);
    //             normals.push_back(nz);
    //         }
    //     }
    // }

    // // 도넛 인덱스 생성 함수
    // void generateTorusIndices(std::vector<GLuint> &indices, int radialSegments, int tubularSegments) {
    //     // 삼각형 인덱스 생성
    //     for (int i = 0; i < radialSegments; ++i) {
    //         for (int j = 0; j < tubularSegments; ++j) {
    //             int first = (i * (tubularSegments + 1)) + j;
    //             int second = first + tubularSegments + 1;

    //             // 첫 번째 삼각형 (CCW 방향)
    //             indices.push_back(first);
    //             indices.push_back(first + 1);
    //             indices.push_back(second);

    //             // 두 번째 삼각형 (CCW 방향)
    //             indices.push_back(first + 1);
    //             indices.push_back(second + 1);
    //             indices.push_back(second);
    //         }
    //     }
    // }

    // void generateTorusIndices(std::vector<GLuint> &indices, int radialSegments, int tubularSegments) {
    //     // 삼각형 인덱스 생성
    //     for (int i = 0; i < radialSegments; ++i) {
    //         for (int j = 0; j < tubularSegments; ++j) {
    //             int first = (i * (tubularSegments + 1)) + j;
    //             int second = first + tubularSegments + 1;

    //             // 첫 번째 삼각형
    //             indices.push_back(first);
    //             indices.push_back(second);
    //             indices.push_back(first + 1);

    //             // 두 번째 삼각형
    //             indices.push_back(second);
    //             indices.push_back(second + 1);
    //             indices.push_back(first + 1);
    //         }
    //     }
    // }


    void initializeGL() override {
        // 도넛의 설정값
        float radius = 0.5f;       // 큰 반지름
        float tubeRadius = 0.25f;   // 작은 반지름
        int radialSegments = 300;   // 큰 원의 세그먼트 수
        int tubularSegments = 300;  // 작은 원의 세그먼트 수

        std::vector<GLfloat> vertices;   // 점 좌표
        std::vector<GLfloat> normals;
        std::vector<GLuint> indices;
        std::vector<GLfloat> texCoords; // 텍스처 좌표
        std::vector<GLfloat> colors = {
            1.0f, 0.0f, 0.0f,  // 빨강
            0.0f, 1.0f, 0.0f,  // 초록
            0.0f, 1.0f, 1.0f   // 파랑
        };

        // 도넛 생성
        generateTorus(vertices, normals, radius, tubeRadius, radialSegments, tubularSegments);
        generateTorusIndices(indices, radialSegments, tubularSegments);
        // generateTorus(vertices, normals, radius, tubeRadius, radialSegments, tubularSegments);
        // generateTorusIndices(indices, radialSegments, tubularSegments);

        // 결과 출력 (테스트용)
        qDebug() << "Vertices: " << vertices.size() / 3 << " points\n";
        qDebug() << "Normals: " << normals.size() / 3 << " points\n";
        qDebug() << "Indices: " << indices.size() / 3 << " triangles\n";

        openglCore = new MyOpenGLCore(vertices, indices, normals);
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





