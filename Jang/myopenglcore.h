#ifndef MYOPENGLCORE_H
#define MYOPENGLCORE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QString>
#include <vector>

class MyOpenGLCore : public QOpenGLFunctions_4_0_Core
{
public:
    // 생성자 (Vertex, Color, Texture 좌표)
    explicit MyOpenGLCore(const std::vector<GLfloat> &vertices,
                   const std::vector<GLfloat> &colors = {},
                   const std::vector<GLfloat> &texCoords = {});
    ~MyOpenGLCore();

    void initialize();
    void render();
    bool loadShaders(const QString &vertexShaderPath, const QString &fragmentShaderPath);
    void activeVertexInputAttributesAndIndices();
    void activeUniformVariables();
    void checkDefaults();


private:
    GLuint m_program;   // 셰이더 프로그램 ID
    GLuint m_vao;       // VAO 핸들
    GLuint m_vbo;       // VBO 핸들 (정점)
    GLuint m_cbo;       // CBO 핸들 (색상)
    GLuint m_tbo;   // Texture Coordinate Buffer Object

    std::vector<GLfloat> m_vertices;   // 정점 데이터
    std::vector<GLfloat> m_colors;     // 색상 데이터
    std::vector<GLfloat> m_texCoords;

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
};

#endif // MYOPENGLCORE_H
