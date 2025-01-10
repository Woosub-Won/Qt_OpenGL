#ifndef MYOPENGLCORE_H
#define MYOPENGLCORE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QString>
#include <vector>

class MyOpenGLCore : public QOpenGLFunctions_4_0_Core
{
public:
    explicit MyOpenGLCore(const std::vector<GLfloat> &vertices,
                          const std::vector<GLfloat> &colors);
    ~MyOpenGLCore();

    void initialize();
    void render();
    bool loadShaders(const QString &vertexShaderPath, const QString &fragmentShaderPath);
    void activeVertexInputAttributesAndIndices();

private:
    GLuint m_program;                  // OpenGL 셰이더 프로그램 ID
    QOpenGLVertexArrayObject m_vao;    // VAO
    QOpenGLBuffer m_vbo;               // VBO
    QOpenGLBuffer m_cbo;               // CBO

    std::vector<GLfloat> m_vertices;   // 정점 데이터
    std::vector<GLfloat> m_colors;     // 색상 데이터

    void checkShaderCompileStatus(GLuint shader);
    void checkProgramLinkStatus(GLuint program);
};

#endif // MYOPENGLCORE_H
