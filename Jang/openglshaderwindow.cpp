#include "OpenGLShaderWindow.h"
#include <QSurfaceFormat>
#include <QScreen>
#include <QExposeEvent>
#include <QEvent>
#include <cmath>

// 정적 셰이더 소스
const char *OpenGLShaderWindow::vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

const char *OpenGLShaderWindow::fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

// 생성자
OpenGLShaderWindow::OpenGLShaderWindow(QWindow *parent)
    : QWindow(parent)
{
    // OpenGL 서피스 타입 설정
    setSurfaceType(OpenGLSurface);
}

// 소멸자
OpenGLShaderWindow::~OpenGLShaderWindow()
{
    delete m_program;
    delete m_device;
    // m_context는 Qt가 소멸 시 관리 (부모가 this)
}

// OpenGL 필수 초기화 함수
void OpenGLShaderWindow::initialize()
{
    // 삼각형 데이터 초기화
    initTriangleData();
}

// QPainter 기반의 렌더링 (옵션)
void OpenGLShaderWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
    // 필요하다면 QPainter로 2D 그리기 가능
}

// OpenGL 직접 호출을 통한 렌더링
void OpenGLShaderWindow::render()
{
    // 여기서 OpenGL 그리기
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    // 셰이더 프로그램 바인딩
    m_program->bind();

    // 변환 행렬
    QMatrix4x4 matrix;
    matrix.perspective(60.0f, (float)width() / (float)height(), 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    // 화면 주사율에 따른 삼각형 회전
    if (screen()->refreshRate() > 0) {
        matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
    } else {
        // 혹시 refreshRate가 0이면 임의 회전
        matrix.rotate(m_frame, 0, 1, 0);
    }

    // 셰이더의 uniform에 행렬 전달
    m_program->setUniformValue(m_matrixUniform, matrix);

    m_vao.bind(); // VAO 바인딩
    // 삼각형 그리기
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_vao.release(); // VAO 해제

    m_program->release();

    ++m_frame;
}

// 삼각형 버텍스/컬러 데이터 초기화
void OpenGLShaderWindow::initTriangleData()
{
    // 삼각형 정점 + 색상 정보
    static const GLfloat vertices[] = {
        // (x, y),   (r, g, b)
        0.0f,  0.707f,
        -0.5f, -0.500f,
        0.5f, -0.500f,
    };
    // 삼각형 정점 + 색상 정보
    static const GLfloat colors[] = {
        // (x, y),   (r, g, b)
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    // VAO 생성 및 바인딩
    m_vao.create();
    m_vao.bind();
    // QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // VBO 생성
    m_vbo_vertex.create();
    m_vbo_vertex.bind();
    m_vbo_vertex.allocate(vertices, sizeof(vertices));
    m_vbo_color.create();
    m_vbo_color.bind();
    m_vbo_color.allocate(colors, sizeof(colors));

    // posAttr
    m_vbo_vertex.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    // colAttr
    m_vbo_color.bind();
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // 셰이더 프로그램 생성
    m_program = new QOpenGLShaderProgram(this);
    // m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    // m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../../tmp.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../../tmp.frag");
    m_program->bindAttributeLocation("posAttr", 0);
    m_program->bindAttributeLocation("colAttr", 1);
    m_program->link();
    m_program->bind();

    // uniform 위치 찾기
    m_matrixUniform = m_program->uniformLocation("matrix");
}

// 윈도우 갱신 요청
void OpenGLShaderWindow::renderLater()
{
    requestUpdate();
}

// 실제 렌더링 발생 지점
void OpenGLShaderWindow::renderNow()
{
    // 창이 노출되지 않은 경우
    if (!isExposed())
        return;

    bool needsInitialize = false;

    // 아직 OpenGLContext가 없으면 생성
    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    // 현재 윈도우에 컨텍스트를 연결
    m_context->makeCurrent(this);

    // 첫 초기화 시점에 한 번만
    if (needsInitialize) {
        initializeOpenGLFunctions(); // QOpenGLFunctions 초기화
        initialize();               // 사용자 정의 초기화
    }

    // 렌더링 수행
    render();

    // 화면에 결과 반영
    m_context->swapBuffers(this);

    // 애니메이션 모드면 다시 요청
    if (m_animating)
        renderLater();
}

// 애니메이션 모드 on/off
void OpenGLShaderWindow::setAnimating(bool animating)
{
    m_animating = animating;
    if (animating)
        renderLater();
}

// 일반 이벤트 처리
bool OpenGLShaderWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

// 노출 이벤트 처리
void OpenGLShaderWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}
