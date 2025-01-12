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
    uniformBuffer.destroy();
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

    GLuint program = shaderProgram.programId();

    // 사각형 메쉬 데이터 정의
    float vertices[] = {
        // 위치 (x, y, z)
        -1.0f, -1.0f, 0.0f,  // 왼쪽 아래
         1.0f, -1.0f, 0.0f,  // 오른쪽 아래
         1.0f,  1.0f, 0.0f,  // 오른쪽 위
        -1.0f,  1.0f, 0.0f   // 왼쪽 위
    };

    float texCoords[] = {
        0.0f, 0.0f, 0.0f,   // 왼쪽 아래
        1.0f, 0.0f, 0.0f,    // 오른쪽 아래
        1.0f, 1.0f, 0.0f,    // 오른쪽 위
        0.0f, 1.0f, 0.0f     // 왼쪽 위
    };

    unsigned int indices[] = {
        0, 1, 2,    // 첫 번째 삼각형
        0, 2, 3     // 두 번째 삼각형
    };

    // VAO 및 버퍼 설정
    VAO.create();
    VAO.bind();

    // 위치 데이터를 위한 VBO
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(vertices, sizeof(vertices));
    GLuint vertexPosition = glGetAttribLocation(program, "VertexPosition");
    glEnableVertexAttribArray(vertexPosition);
    glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // 색상 데이터를 위한 VBO
    colorBuffer.create();
    colorBuffer.bind();
    colorBuffer.allocate(texCoords, sizeof(texCoords));
    GLuint vertexColor = glGetAttribLocation(program, "VertexColor");
    glEnableVertexAttribArray(vertexColor);
    glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // 인덱스 버퍼 (IBO)
    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate(indices, sizeof(indices));


    // Uniform Buffer
    uniformData.innerColor = QVector4D(1.0f, 1.0f, 0.75f, 1.0f);
    uniformData.outerColor = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
    uniformData.innerRadius = 0.25f;
    uniformData.outerRadius = 0.45f;

    uniformBuffer.create();
    uniformBuffer.bind();
    uniformBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    uniformBuffer.allocate(&uniformData, sizeof(UniformBlock));

    GLuint blockIndex = glGetUniformBlockIndex(shaderProgram.programId(), "BlobSettings");
    glUniformBlockBinding(shaderProgram.programId(), blockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer.bufferId());    

    VAO.release();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rotationMatrix.rotate(1.0f, 0.0f, 0.0f, 1.0f);

    uniformBuffer.bind();
    uniformBuffer.write(0, &uniformData, sizeof(UniformBlock));

    GLuint program = shaderProgram.programId();
    shaderProgram.bind();
    VAO.bind();

    GLuint RM = glGetUniformLocation(program, "RotationMatrix");
    glUniformMatrix4fv(RM, 1, GL_FALSE, rotationMatrix.constData());

    // 인덱스 버퍼를 사용하여 그리기
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    VAO.release();
    shaderProgram.release();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}