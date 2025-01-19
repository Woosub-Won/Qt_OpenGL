#ifndef MYOPENGLCORE_H
#define MYOPENGLCORE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QString>
#include <QImage>
#include <vector>

#include <QQuaternion>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMatrix4x4>

class MyOpenGLCore : public QOpenGLFunctions_4_0_Core
{
public:
    // 생성자 오버로드는 최소화하고,
    // 파일 경로만 저장하는 생성자를 사용하도록 일원화
    explicit MyOpenGLCore(const QString &objFilePath = QString(),
                          const QString &textureFilePath = QString());

    ~MyOpenGLCore();

    // 초기화(리소스 로딩 포함)
    void initialize(const QString &vertexShaderPath,
                    const QString &fragmentShaderPath);

    // 각종 이벤트 처리
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);

    // 매 프레임마다 호출 (렌더링)
    void render();

    // 디버깅용: 활성화된 정점 속성과 유니폼 출력
    void activeVertexInputAttributesAndIndices();
    void activeUniformVariables();

private:
    // 내부 동작 분리
    void loadModel();
    void loadTexture();
    void loadTexture2();
    void createBuffers();
    void configureTexture();
    void configureTexture2();
    void loadShaders(const QString &vertexShaderPath,
                     const QString &fragmentShaderPath);
    bool compileShader(GLuint shader, const char* source);
    bool linkProgram(GLuint program);

    // OBJ 파싱 로직(기존 parseObjFile)
    bool parseObjFile(const QString &filePath);
    void generateNormals(const std::vector<GLfloat> &vertices,
                         const std::vector<GLuint> &indices,
                         std::vector<GLfloat> &normals);
    void centerObjectAtOrigin();

    // 카메라/조명/행렬 설정
    void setMatrices();
    void updateCameraView(QMatrix4x4 &viewMatrix);

    // 셰이더 컴파일, 링크 상태 체크
    void checkShaderCompileStatus(GLuint shader);
    void checkProgramLinkStatus(GLuint program);

private:
    // 셰이더, 버퍼 관련
    GLuint m_program;
    GLuint m_vao;
    GLuint m_vbo;  // 정점
    GLuint m_nbo;  // 법선
    GLuint m_cbo;  // 색상
    GLuint m_tbo;  // 텍스처 좌표
    GLuint m_ebo;  // 인덱스
    GLuint m_texture;
    GLuint m_texture2;

    // obj, texture 경로
    QString m_objFilePath;
    QString m_textureFilePath;
    QString m_textureFilePath2;

    // 로드된 메쉬 데이터
    std::vector<GLfloat> m_vertices;
    std::vector<GLfloat> m_normals;
    std::vector<GLfloat> m_colors;
    std::vector<GLfloat> m_texCoords;
    std::vector<GLuint>  m_indices;

    // 텍스처 이미지
    QImage m_textureImage;
    QImage m_textureImage2;

    // 카메라, 이동
    QVector3D    m_cameraPosition;
    QQuaternion  m_cameraRotation;
    QPoint       m_lastMousePosition;
    bool         m_isMousePressed;
    float        m_moveSpeed;
    float        m_rotationSpeed;

    // 셰이더에서 사용할 Uniform 위치
    GLint m_uniformLightPosition;
    GLint m_uniformModelViewMatrix;
    GLint m_uniformNormalMatrix;
    GLint m_uniformProjectionMatrix;
    GLint m_uniformMVP;

    // 기본 텍스처/색상 상수
    const std::vector<GLfloat> defaultColors = {
        1.0f, 0.0f, 0.0f, // R
        0.0f, 1.0f, 0.0f, // G
        0.0f, 0.0f, 1.0f  // B
    };
    const std::vector<GLfloat> defaultTexCoords = {
        0.0f, 0.0f, // Bottom-left
        1.0f, 0.0f, // Bottom-right
        1.0f, 1.0f, // Top-right
        0.0f, 1.0f  // Top-left
    };
};

#endif // MYOPENGLCORE_H
