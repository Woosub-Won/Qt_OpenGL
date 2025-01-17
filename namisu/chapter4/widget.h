#ifndef WIDGET_H
#define WIDGET_H

#include <C:/glm-master/glm/glm.hpp>
#include <C:/glm-master/glm/gtc/matrix_transform.hpp>
#include <C:/glm-master/glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <vector>

using namespace glm;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    // 입력 이벤트 처리
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    GLuint shaderProgram; // 셰이더 프로그램 ID
    GLuint VAO, VBO, EBO;;      // VAO와 VBO
    GLuint tid[3];
    float rotationAngle = 0.0f;
    QTimer *timer;             // 애니메이션 타이머
    GLuint loadShader(const char* filePath, GLenum shaderType); // 셰이더 로드 함수
    GLuint InitShader(const char* vertexPath, const char* fragmentPath);


    float radius = 3.5f; // 물체에서 카메라까지의 거리
    vec3 eye = vec3(0.0, 0.0, 5.0); // 초기 카메라 위치
    vec3 at = vec3(0.0, 0.0, 0.0);  // 초기 바라보는 점
    vec3 up = vec3(0.0, 1.0, 0.0);  // 업 벡터

    float yaw = 0.0f, pitch = 0.0f; // 카메라 회전 값 (yaw: 좌우, pitch: 상하)
    float sensitivity = 0.005;
    QPoint prevMousePos = {0, 0};

    bool RButton = false; // 우클릭 상태를 추적하는 변수

    float p_angle = 60.0f;
    float p_near = 0.01f;
    float p_far = 100.0f;

    int drawSize = 0;

    struct Vertex {
        vec4 position;   // 정점 위치
        vec3 normal;    // 정점 법선
        vec2 texCoord;   // 텍스처 좌표 추가
        vec4 tangent;
    };

    Vertex* vertexData;

    vector<vec4> vertices;
    vector<vec4> faces;
    vector<vec4> vertexNormal;
    vector<vec2> texCoords;
    int vertexCount = 0, faceCount = 0, tIndex = 0;
    vec4 center;

    void setUniforms(GLuint program, const mat4& model, const mat4& view, const mat4& projection);
    void initializeBuffers();
    void initializeSphereBuffers(const std::vector<float>& sphereVertices);
    void loadTexture(const char* filename, int idx);
    void updateCameraPosition();

    void ReadOBJ(FILE* file);
    vec4 calculateSurfaceNormal(vec4 v1, vec4 v2, vec4 v3);
};

#endif // WIDGET_H
