#include "openglshaderwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPainter>

OpenGLShaderWindow::OpenGLShaderWindow(QWindow *parent)
    : QWindow(parent), m_context(nullptr), m_device(nullptr), m_program(nullptr), m_animating(false)
{
    setSurfaceType(QWindow::OpenGLSurface);
    qDebug() << "OpenGLShaderWindow created.";
}

OpenGLShaderWindow::~OpenGLShaderWindow()
{
    delete m_device;
    delete m_program;
    qDebug() << "OpenGLShaderWindow destroyed.";
}

void OpenGLShaderWindow::setAnimating(bool animating)
{
    m_animating = animating;
    if (animating)
        renderLater();
}

void OpenGLShaderWindow::initialize(const QString &vertexShaderPath, const QString &fragmentShaderPath)
{
    qDebug() << "Initializing OpenGLShaderWindow with shaders:";
    qDebug() << "Vertex Shader Path:" << vertexShaderPath;
    qDebug() << "Fragment Shader Path:" << fragmentShaderPath;

    m_program = new QOpenGLShaderProgram(this);

    // 셰이더 파일 경로에서 소스 코드 로드
    QString vertexShaderSource = loadShaderSource(vertexShaderPath);
    QString fragmentShaderSource = loadShaderSource(fragmentShaderPath);

    if (vertexShaderSource.isEmpty()) {
        qWarning() << "Failed to load vertex shader from:" << vertexShaderPath;
        return;
    }
    if (fragmentShaderSource.isEmpty()) {
        qWarning() << "Failed to load fragment shader from:" << fragmentShaderPath;
        return;
    }

    // 셰이더 소스 컴파일
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qWarning() << "Failed to compile vertex shader:" << m_program->log();
    } else {
        qDebug() << "Vertex shader compiled successfully.";
    }

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qWarning() << "Failed to compile fragment shader:" << m_program->log();
    } else {
        qDebug() << "Fragment shader compiled successfully.";
    }

    // 프로그램 링크
    if (!m_program->link()) {
        qWarning() << "Failed to link shader program:" << m_program->log();
    } else {
        qDebug() << "Shader program linked successfully.";
    }
}

void OpenGLShaderWindow::render()
{
    if (!m_device)
        m_device = new QOpenGLPaintDevice;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_program) {
        m_program->bind();
        // 렌더링 관련 추가 작업
        m_program->release();
    }

    m_device->setSize(size());
    QPainter painter(m_device);
    painter.end();
}

QString OpenGLShaderWindow::loadShaderSource(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to load shader file:" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    qDebug() << "Shader file loaded successfully:" << filePath;
    return stream.readAll();
}

void OpenGLShaderWindow::renderNow()
{
    if (!isExposed())
        return;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        if (!m_context->create()) {
            qWarning() << "Failed to create OpenGL context.";
            return;
        } else {
            qDebug() << "OpenGL context created successfully.";
        }
    }

    m_context->makeCurrent(this);

    if (!m_device) {
        initializeOpenGLFunctions();
    }

    render();
    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

void OpenGLShaderWindow::renderLater()
{
    requestUpdate();
}

void OpenGLShaderWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    if (isExposed())
        renderNow();
}

bool OpenGLShaderWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        renderNow();
        return true;
    }
    return QWindow::event(event);
}
