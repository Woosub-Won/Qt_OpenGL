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

    // Shader ���α׷� ����
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        qDebug() << "Vertex shader error";
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        qDebug() << "Fragment shader error";
    if (!shaderProgram.link())
        qDebug() << "Shader link error";

    GLuint program = shaderProgram.programId();

    // �簢�� �޽� ������ ����
    float vertices[] = {
        // ��ġ (x, y, z)
        -1.0f, -1.0f, 0.0f,  // ���� �Ʒ�
         1.0f, -1.0f, 0.0f,  // ������ �Ʒ�
         1.0f,  1.0f, 0.0f,  // ������ ��
        -1.0f,  1.0f, 0.0f   // ���� ��
    };

    float texCoords[] = {
        0.0f, 0.0f, 0.0f,   // ���� �Ʒ�
        1.0f, 0.0f, 0.0f,    // ������ �Ʒ�
        1.0f, 1.0f, 0.0f,    // ������ ��
        0.0f, 1.0f, 0.0f     // ���� ��
    };

    unsigned int indices[] = {
        0, 1, 2,    // ù ��° �ﰢ��
        0, 2, 3     // �� ��° �ﰢ��
    };

    // VAO �� ���� ����
    VAO.create();
    VAO.bind();

    // ��ġ �����͸� ���� VBO
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(vertices, sizeof(vertices));
    GLuint vertexPosition = glGetAttribLocation(program, "VertexPosition");
    glEnableVertexAttribArray(vertexPosition);
    glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // ���� �����͸� ���� VBO
    colorBuffer.create();
    colorBuffer.bind();
    colorBuffer.allocate(texCoords, sizeof(texCoords));
    GLuint vertexColor = glGetAttribLocation(program, "VertexColor");
    glEnableVertexAttribArray(vertexColor);
    glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // �ε��� ���� (IBO)
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

    // �ε��� ���۸� ����Ͽ� �׸���
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    VAO.release();
    shaderProgram.release();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}