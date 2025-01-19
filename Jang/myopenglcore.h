#ifndef MYOPENGLCORE_H
#define MYOPENGLCORE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QString>
#include <QImage>
#include <vector>

#include <QQuaternion>
#include <QMouseEvent>
#include <QKeyEvent>

class MyOpenGLCore : public QOpenGLFunctions_4_0_Core
{
public:
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
public:
    // 생성자 (Vertex, Normal, Color, Texture 좌표)
    explicit MyOpenGLCore(const std::vector<GLfloat> &vertices,
                          const std::vector<GLuint> &indices = {},
                          const std::vector<GLfloat> &normals = {});
    explicit MyOpenGLCore(const QString &objFilePath);
    explicit MyOpenGLCore(const QString &objFilePath, const QString &texturePath);

    ~MyOpenGLCore();

    void initialize();
    void render();
    bool loadShaders(const QString &vertexShaderPath, const QString &fragmentShaderPath);
    void activeVertexInputAttributesAndIndices();
    void activeUniformVariables();
    void checkDefaults();
    bool parseObjFile(const QString &filePath);
private:
    GLuint m_program;   // 셰이더 프로그램 ID
    GLuint m_vao;       // VAO 핸들
    GLuint m_vbo;       // VBO 핸들 (정점)
    GLuint m_nbo;       // NBO 핸들 (법선)
    GLuint m_cbo;       // CBO 핸들 (색상)
    GLuint m_tbo;       // TBO 핸들 (텍스처)
    GLuint m_ebo;       // EBO 핸들 (인덱스)

    std::vector<GLfloat> m_vertices;   // 정점 데이터
    std::vector<GLfloat> m_normals;    // 법선 데이터
    std::vector<GLfloat> m_colors;     // 색상 데이터
    std::vector<GLfloat> m_texCoords;  // 텍스처 좌표 데이터
    std::vector<GLuint> m_indices; // 인덱스 데이터
    QImage m_textureImage;
    GLuint m_texture; // 텍스처 핸들을 분리

    // 초기 기본값 (상수)
    const std::vector<GLfloat> defaultColors = {
        1.0f, 0.0f, 0.0f,  // 빨강
        0.0f, 1.0f, 0.0f,  // 초록
        0.0f, 1.0f, 1.0f   // 파랑
    };

    const std::vector<GLfloat> defaultTexCoords = {
        0.0f, 0.0f,  // Bottom-left
        1.0f, 0.0f,  // Bottom-right
        1.0f, 1.0f,  // Top-right
        0.0f, 1.0f   // Top-left
    };
    void checkShaderCompileStatus(GLuint shader);
    void checkProgramLinkStatus(GLuint program);

private:
    GLint m_uniformLightPosition;  // Light position
    GLint m_uniformKd;             // Diffuse reflectivity
    GLint m_uniformLd;             // Light source intensity
    GLint m_uniformModelViewMatrix;
    GLint m_uniformNormalMatrix;
    GLint m_uniformProjectionMatrix;
    GLint m_uniformMVP;            // Projection * ModelView matrix
    void generateNormals(const std::vector<GLfloat> &vertices, const std::vector<GLuint> &indices, std::vector<GLfloat> &normals);
private:
    QVector3D m_cameraPosition = QVector3D(0.0f, 0.0f, 5.0f); // 초기 카메라 위치
    QQuaternion m_cameraRotation;                             // 카메라 회전
    QPoint m_lastMousePosition;                               // 마지막 마우스 위치
    bool m_isMousePressed = false;                            // 마우스 클릭 상태
    float m_moveSpeed = 0.1f;                                 // 이동 속도
    float m_rotationSpeed = 0.2f;                             // 회전 속도

    void updateCameraView(QMatrix4x4 &viewMatrix);
    void centerObjectAtOrigin();
};

#endif // MYOPENGLCORE_H
