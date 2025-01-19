#include "glwidget.h"
#include <GL/freeglut.h>

GLWidget::GLWidget(QWidget* parent, Qt::WindowFlags f) :
    QOpenGLWidget(parent, f),
    positionBuffer(QOpenGLBuffer::VertexBuffer),
    normalBuffer(QOpenGLBuffer::VertexBuffer),
    texBuffer(QOpenGLBuffer::VertexBuffer),
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
    texBuffer.destroy();
    indexBuffer.destroy();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // 배경색 설정
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // 조명 설정
    lightPosition = QVector4D(0.0f, 50.0f, 10.0f, 1.0f);
    lightIntensity = QVector3D(0.7f, 0.7f, 0.7f);
    lightAmbient = QVector3D(0.3f, 0.3f, 0.3f);    // ambient를 조금 높임
    lightDiffuse = QVector3D(0.7f, 0.7f, 0.7f);    // diffuse를 조금 낮춤
    lightSpecular = QVector3D(0.5f, 0.5f, 0.5f);   // specular도 조절

    // 재질 설정
    materialAmbient = QVector3D(0.7f, 0.4f, 0.2f);   // 구리 색상
    materialDiffuse = QVector3D(0.7f, 0.4f, 0.2f);
    materialSpecular = QVector3D(1.0f, 1.0f, 1.0f);
    materialShininess = 32.0f;

    // MVP 행렬 설정
    modelViewMatrix.setToIdentity();
    modelViewMatrix.lookAt(QVector3D(0, 30, 30), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, width() / float(height()), 0.1f, 100.0f);

    // Depth Test 활성화
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);  // 후면 컬링 활성화
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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

    /*texBuffer.create();
    texBuffer.bind();
    texBuffer.allocate(texCoord.data(), texCoord.size() * sizeof(float));
    GLuint vertexTex = shaderProgram.attributeLocation("VertexTexCoord");
    shaderProgram.enableAttributeArray(vertexTex);
    shaderProgram.setAttributeBuffer(vertexTex, GL_FLOAT, 0, 3, 0);*/

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


    // 공통 사용 uniform
    shaderProgram.setUniformValue("LightPosition", lightPosition);
    shaderProgram.setUniformValue("LightIntensity", lightIntensity);
    shaderProgram.setUniformValue("Shininess", 64.0f);

    shaderProgram.setUniformValue("Ka", materialAmbient);
    shaderProgram.setUniformValue("Kd", materialDiffuse);
    shaderProgram.setUniformValue("Ks", materialSpecular);

    QMatrix4x4 model;
    model.rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 mv = modelViewMatrix * model;
    QMatrix4x4 mvp = projectionMatrix * mv;

    // MVP 행렬 설정
    shaderProgram.setUniformValue("ModelViewMatrix", mv);
    shaderProgram.setUniformValue("NormalMatrix", mv.normalMatrix());
    shaderProgram.setUniformValue("ProjectionMatrix", projectionMatrix);
    shaderProgram.setUniformValue("MVP", mvp);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    

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

    std::vector<float> tmp_v;
    std::vector<float> tmp_n;
    std::vector<float> tmp_t;
    std::vector<int> idx_v;
    std::vector<int> idx_n;
    std::vector<int> idx_t;

    std::string line;
    std::string idx1, idx2, idx3, idx4;
    int i11, i12, i13;
    int i21, i22, i23;
    int i31, i32, i33;
    int i41, i42, i43;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);   // 공백을 기준으로 파싱
        std::string token;
        iss >> token;

        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            tmp_v.push_back(x);
            tmp_v.push_back(y);
            tmp_v.push_back(z);
        }
        else if (token == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            tmp_n.push_back(x);
            tmp_n.push_back(y);
            tmp_n.push_back(z);
        }
        else if (token == "vt")
        {
            float x, y, z;
            iss >> x >> y >> z;
            tmp_t.push_back(x);
            tmp_t.push_back(y);
            tmp_t.push_back(z);
        }
        else if (token == "f")
        {
            iss >> idx1 >> idx2 >> idx3 >> idx4;

            // idx1 처리
            std::stringstream iss1(idx1);
            std::string token;
            getline(iss1, token, '/');
            i11 = stoi(token);
            getline(iss1, token, '/');
            i12 = stoi(token);
            getline(iss1, token, '/');
            i13 = stoi(token);

            // idx2 처리
            std::stringstream iss2(idx2);
            getline(iss2, token, '/');
            i21 = stoi(token);
            getline(iss2, token, '/');
            i22 = stoi(token);
            getline(iss2, token, '/');
            i23 = stoi(token);

            // idx3 처리
            std::stringstream iss3(idx3);
            getline(iss3, token, '/');
            i31 = stoi(token);
            getline(iss3, token, '/');
            i32 = stoi(token);
            getline(iss3, token, '/');
            i33 = stoi(token);


            idx_v.push_back(i11 - 1);   idx_v.push_back(i21 - 1);   idx_v.push_back(i31 - 1);
            idx_t.push_back(i12 - 1);   idx_t.push_back(i22 - 1);   idx_t.push_back(i32 - 1);
            idx_n.push_back(i13 - 1);   idx_n.push_back(i23 - 1);   idx_n.push_back(i33 - 1);
        }
    }
    
    for (int i = 0; i < idx_v.size(); i++)
    {
        vertices.push_back(tmp_v[idx_v[i] * 3 + 0]);
        vertices.push_back(tmp_v[idx_v[i] * 3 + 1]);
        vertices.push_back(tmp_v[idx_v[i] * 3 + 2]);

        normals.push_back(tmp_n[idx_n[i] * 3 + 0]);
        normals.push_back(tmp_n[idx_n[i] * 3 + 1]);
        normals.push_back(tmp_n[idx_n[i] * 3 + 2]);

        texCoord.push_back(tmp_t[idx_t[i] * 3 + 0]);
        texCoord.push_back(tmp_t[idx_t[i] * 3 + 1]);
        texCoord.push_back(tmp_t[idx_t[i] * 3 + 2]);

    }

    for (int i = 0; i < idx_v.size(); i++)
    {
        indices.push_back(i);
    }
}