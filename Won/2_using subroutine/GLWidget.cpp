#include "glwidget.h"
#include <GL/freeglut.h>

GLWidget::GLWidget(QWidget* parent, Qt::WindowFlags f) :
    QOpenGLWidget(parent, f),
    positionBuffer(QOpenGLBuffer::VertexBuffer),
    normalBuffer(QOpenGLBuffer::VertexBuffer),
    indexBuffer(QOpenGLBuffer::IndexBuffer)
{

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(16);
    rotationAngle = 0.0f;
}

GLWidget::~GLWidget()
{
    makeCurrent();
    VAO.destroy();
    positionBuffer.destroy();
    normalBuffer.destroy();
    indexBuffer.destroy();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // 배경색 설정
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // 라이트 설정
    lightPosition = QVector4D(5.0f, 5.0f, 5.0f, 1.0f);
    lightAmbient = QVector3D(0.2f, 0.2f, 0.2f);
    lightDiffuse = QVector3D(1.0f, 1.0f, 1.0f);
    lightSpecular = QVector3D(1.0f, 1.0f, 1.0f);

    // 재질 설정
    materialAmbient = QVector3D(0.7f, 0.4f, 0.2f);   // 구리 색상
    materialDiffuse = QVector3D(0.7f, 0.4f, 0.2f);
    materialSpecular = QVector3D(1.0f, 1.0f, 1.0f);
    materialShininess = 32.0f;

    // MVP 행렬 설정
    modelViewMatrix.setToIdentity();
    modelViewMatrix.lookAt(QVector3D(0, 10, 10), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, width() / float(height()), 0.1f, 100.0f);

    // Depth Test 활성화
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // 후면 컬링 활성화

    // Shader 프로그램 설정
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        qDebug() << "Vertex shader error";
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        qDebug() << "Fragment shader error";
    if (!shaderProgram.link())
        qDebug() << "Shader link error";

    readObjFile("teapot.obj");

    // VAO 및 버퍼 설정
    VAO.create();
    VAO.bind();

    // 위치 데이터를 위한 VBO
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(vertices.data(), vertices.size() * sizeof(float));
    GLuint vertexPosition = shaderProgram.attributeLocation("VertexPosition");
    shaderProgram.enableAttributeArray(vertexPosition);
    shaderProgram.setAttributeBuffer(vertexPosition, GL_FLOAT, 0, 3, 0);

    // 색상 데이터를 위한 VBO
    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.allocate(normals.data(), normals.size() * sizeof(float));
    GLuint vertexNormal = shaderProgram.attributeLocation("VertexNormal");
    shaderProgram.enableAttributeArray(vertexNormal);
    shaderProgram.setAttributeBuffer(vertexNormal, GL_FLOAT, 0, 3, 0);

    // 인덱스 버퍼 (IBO)
    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate(indices.data(), indices.size() * sizeof(int));

    VAO.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    shaderProgram.bind();
    VAO.bind();

    GLuint programId = shaderProgram.programId();
    adsIndex = this->glGetSubroutineIndex(programId, GL_VERTEX_SHADER, "phongModel");
    diffuseIndex = this->glGetSubroutineIndex(programId, GL_VERTEX_SHADER, "diffuseOnly");


    // 공통 사용 uniform
    shaderProgram.setUniformValue("Light.Position", lightPosition);
    shaderProgram.setUniformValue("Light.La", lightAmbient);
    shaderProgram.setUniformValue("Light.Ld", lightDiffuse);
    shaderProgram.setUniformValue("Light.Ls", lightSpecular);

    shaderProgram.setUniformValue("Material.Ka", materialAmbient);
    shaderProgram.setUniformValue("Material.Kd", materialDiffuse);
    shaderProgram.setUniformValue("Material.Ks", materialSpecular);
    shaderProgram.setUniformValue("Material.Shininess", materialShininess);

    // 첫 번째 주전자 (Phong 모델)
    {
        QMatrix4x4 model;
        model.translate(-3.0f, 0.0f, 0.0f);
        model.rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
        QMatrix4x4 mv = modelViewMatrix * model;
        QMatrix4x4 mvp = projectionMatrix * mv;

        // MVP 행렬 설정
        shaderProgram.setUniformValue("ModelViewMatrix", mv);
        shaderProgram.setUniformValue("NormalMatrix", mv.normalMatrix());
        shaderProgram.setUniformValue("MVP", mvp);


        // Phong 모델 서브루틴 설정
        this->glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &adsIndex);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    // 두 번째 주전자 (Diffuse only 모델)
    {
        QMatrix4x4 model;
        model.translate(3.0f, 0.0f, 0.0f);
        model.rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
        QMatrix4x4 mv = modelViewMatrix * model;
        QMatrix4x4 mvp = projectionMatrix * mv;

        shaderProgram.setUniformValue("ModelViewMatrix", mv);
        shaderProgram.setUniformValue("NormalMatrix", mv.normalMatrix());
        shaderProgram.setUniformValue("MVP", mvp);

        // Diffuse only 서브루틴 설정
        this->glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &diffuseIndex);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    rotationAngle += 1.0f;

    VAO.release();
    shaderProgram.release();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::readObjFile(const char* fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);   // 공백을 기준으로 파싱
        std::string token;
        iss >> token;

        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            float size = sqrt(x * x + y * y + z * z);
            normals.push_back(x / size);
            normals.push_back(y / size);
            normals.push_back(z / size);
        }
        else if (token == "f")
        {
            int idx1, idx2, idx3;
            iss >> idx1 >> idx2 >> idx3;
            indices.push_back(idx1 - 1);
            indices.push_back(idx2 - 1);
            indices.push_back(idx3 - 1);
        }
    }
}