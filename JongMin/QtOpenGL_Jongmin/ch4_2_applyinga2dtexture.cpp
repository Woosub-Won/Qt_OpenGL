#include "ch4_2_ApplyingA2DTexture.h"

#include <QImage>
#include <QFile>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <cmath>
#include <QOpenGLTexture>


void ch4_2_ApplyingA2DTexture::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glDepthFunc(GL_LESS);    // 가까운 픽셀을 렌더링
    glDisable(GL_CULL_FACE); // 면 컬링 비활성화

    if (!objreader.load("C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Models/teapot.obj")) {
        qWarning("Failed to load OBJ file.");
        return;
    }
    const auto& vertexData = objreader.getVertexData();
    QVector<QVector3D> positions;
    QVector<QVector3D> normals;
    QVector<QVector2D> texCoords;

    for (const auto& vertex : vertexData) {
        positions.append(vertex.position);
        normals.append(vertex.normal);
        texCoords.append(vertex.texCoord);
    }

    texture = new QOpenGLTexture(QImage("C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Textures/brick1.jpg").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);

    VAO.create();
    VAO.bind();

    pVBO.create();
    pVBO.bind();
    pVBO.allocate(positions.constData(), positions.size() * sizeof(QVector3D));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    pVBO.release();

    nVBO.create();
    nVBO.bind();
    nVBO.allocate(normals.constData(),normals.size() * sizeof(QVector3D));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    nVBO.release();

    texVBO.create();
    texVBO.bind();
    texVBO.allocate(texCoords.constData(), texCoords.size() * sizeof(QVector2D));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    texVBO.release();

    VAO.release();


    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Shaders/vertShader4_2.vert") ||
        !shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Shaders/fragShader4_2.frag") ||
        !shaderProgram.link()) {
        qWarning() << "Shader program initialization failed:" << shaderProgram.log();
        return;
    }
    shaderProgram.bind();

    // GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    // GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // const GLchar* vertShaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/vertShader3_7.vert");
    // glShaderSource(vertShader, 1, &vertShaderCode, nullptr);
    // glCompileShader(vertShader);

    // const GLchar* fragShaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/fragShader3_7.frag");
    // glShaderSource(fragShader, 1, &fragShaderCode, nullptr);
    // glCompileShader(fragShader);

    // programHandle = glCreateProgram();
    // glAttachShader(programHandle, vertShader);
    // glAttachShader(programHandle, fragShader);
    // glLinkProgram(programHandle);

    // glDeleteShader(vertShader);
    // glDeleteShader(fragShader);

    //glUseProgram(programHandle); // 셰이더 활성화

    QVector3D lightPosition(5.0f, 5.0f, 5.0f);
    QVector3D lightIntensity(1.0f, 1.0f, 1.0f); // White light

    shaderProgram.setUniformValue("Light.Position", QVector4D(lightPosition, 1.0f)); // Point light
    shaderProgram.setUniformValue("Light.Intensity", lightIntensity);

    QVector3D Ka(0.2f, 0.2f, 0.2f); // Ambient reflectivity
    QVector3D Kd(0.8f, 0.8f, 0.8f); // Diffuse reflectivity
    QVector3D Ks(1.0f, 1.0f, 1.0f); // Specular reflectivity
    float shininess = 50.0f;

    shaderProgram.setUniformValue("Material.Ka", Ka);
    shaderProgram.setUniformValue("Material.Kd", Kd);
    shaderProgram.setUniformValue("Material.Ks", Ks);
    shaderProgram.setUniformValue("Material.Shininess", shininess);

    QMatrix4x4 modelMatrix;

    modelMatrix.translate(0.0f, 0.0f, -5.0f);
    modelMatrix.rotate(50.f,1.0f,1.0f,0.0f);

    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, width() / height(), 0.1f, 100.0f);

    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvpMatrix = projectionMatrix * modelViewMatrix;

    shaderProgram.setUniformValue("ModelViewMatrix", modelViewMatrix);
    shaderProgram.setUniformValue("NormalMatrix", normalMatrix);
    shaderProgram.setUniformValue("ProjectionMatrix", projectionMatrix);
    shaderProgram.setUniformValue("MVP", mvpMatrix);

    shaderProgram.release();
}

void ch4_2_ApplyingA2DTexture::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 셰이더 프로그램 사용
    shaderProgram.bind();
    texture->bind(0);
    shaderProgram.setUniformValue("Tex1", 0);
    // VAO 바인딩 및 렌더링
    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, objreader.getVertexData().size());
    VAO.release();
    qDebug() << "Number of vertices:" << objreader.getVertexData().size();
    shaderProgram.release();

}


const char *ch4_2_ApplyingA2DTexture::loadShaderAsString(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open shader file:" << filePath;
        return nullptr;
    }

    QTextStream in(&file);
    QString shaderCode = in.readAll();
    file.close();

    // QString을 UTF-8로 변환 후 const char* 반환
    QByteArray shaderCodeBytes = shaderCode.toUtf8();
    return strdup(shaderCodeBytes.constData()); // strdup 사용으로 메모리 관리
}


