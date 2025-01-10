#ifndef OPENGLSHADERWINDOW_H
#define OPENGLSHADERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QPainter>
#include <QOpenGLVertexArrayObject>
#include <vector>

class OpenGLShaderWindow : public QWindow, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT
public:
    explicit OpenGLShaderWindow(QWindow *parent = nullptr);
    OpenGLShaderWindow(const std::vector<GLfloat> &vertices, const std::vector<GLfloat> &colors, QWindow *parent = nullptr);
    ~OpenGLShaderWindow();

    // 렌더링 관련 함수
    void renderLater();
    void renderNow();

    // 애니메이션 on/off
    void setAnimating(bool animating);

    // 기본 초기화 (OpenGL 환경 설정)
    virtual void initialize();

    // QPainter를 통한 렌더링 (필요하다면 재정의)
    virtual void render(QPainter *painter);

    // OpenGL 직접 호출을 통한 렌더링
    virtual void render();

    std::vector<GLfloat> m_vertices; // 버텍스 데이터
    std::vector<GLfloat> m_colors;  // 컬러 데이터

protected:
    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    // 삼각형 렌더링 예시를 위한 내부 셰이더/버퍼 관련
    void initTriangleData();

private:
    bool m_animating = false;

    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;

    // 삼각형 회전용
    int m_frame = 0;

    // VBO
    QOpenGLBuffer m_vbo_vertex;
    QOpenGLBuffer m_vbo_color;

    // VAO
    QOpenGLVertexArrayObject m_vao;

    // 셰이더 프로그램 관련
    QOpenGLShaderProgram *m_program = nullptr;
    GLint m_matrixUniform = -1;

    // 간단한 버텍스 셰이더와 프래그먼트 셰이더 소스
    static const char *vertexShaderSource;
    static const char *fragmentShaderSource;
};

#endif // OPENGLSHADERWINDOW_H
