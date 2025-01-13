#include "glwidget.h"
#include <GL/freeglut.h>

GLWidget::GLWidget(QWidget* parent, Qt::WindowFlags f) :
    QOpenGLWidget(parent, f),
    positionBuffer(QOpenGLBuffer::VertexBuffer),
    colorBuffer(QOpenGLBuffer::VertexBuffer),
    indexBuffer(QOpenGLBuffer::IndexBuffer)    
{
    rotationMatrix.setToIdentity();

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(16);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    VAO.destroy();
    positionBuffer.destroy();
    colorBuffer.destroy();
    indexBuffer.destroy();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Shader 프로그램 설정
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        qDebug() << "Vertex shader error";
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        qDebug() << "Fragment shader error";
    if (!shaderProgram.link())
        qDebug() << "Shader link error";

    // 버텍스, 컬러 데이터 및 인덱스 정의
    float vertices[] = {
        // 위치              
        -0.8f, -0.8f, 0.0f,  // vertex 0
         0.8f, -0.8f, 0.0f,  // vertex 1
         0.0f,  0.8f, 0.0f,  // vertex 2
    };

    float colors[] = {
        1.0f, 0.0f, 0.0f,    // red   (vertex 0)
        0.0f, 1.0f, 0.0f,    // green (vertex 1)
        0.0f, 0.0f, 1.0f     // blue  (vertex 2)
    };

    unsigned int indices[] = {
        0, 1, 2  // 삼각형을 구성하는 버텍스 인덱스
    };

    // VAO 및 버퍼 설정
    VAO.create();
    VAO.bind();

    // 위치 데이터를 위한 VBO
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(vertices, sizeof(vertices));
    GLuint vertexPosition = shaderProgram.attributeLocation("VertexPosition");
    shaderProgram.enableAttributeArray(vertexPosition);
    shaderProgram.setAttributeBuffer(vertexPosition, GL_FLOAT, 0, 3, 3 * sizeof(float));

    // 색상 데이터를 위한 VBO
    colorBuffer.create();
    colorBuffer.bind();
    colorBuffer.allocate(colors, sizeof(colors));
    GLuint vertexColor = shaderProgram.attributeLocation("VertexColor");
    shaderProgram.enableAttributeArray(vertexColor);
    shaderProgram.setAttributeBuffer(vertexColor, GL_FLOAT, 0, 3, 3 * sizeof(float));

    // 인덱스 버퍼 (IBO)
    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate(indices, sizeof(indices));

    VAO.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rotationMatrix.rotate(1.0f, 0.0f, 0.0f, 1.0f);

    GLuint RM = shaderProgram.uniformLocation("RotationMatrix");
    shaderProgram.bind();
    VAO.bind();

    shaderProgram.setUniformValue(RM, rotationMatrix);
    // 인덱스 버퍼를 사용하여 그리기
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    VAO.release();
    shaderProgram.release();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}