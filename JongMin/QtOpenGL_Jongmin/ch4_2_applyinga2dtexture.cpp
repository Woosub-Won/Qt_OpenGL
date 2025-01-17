#include "ch4_2_ApplyingA2DTexture.h"

#include <QImage>
#include <QFile>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <vector>
#include <cmath>
#include <QOpenGLTexture>

void ch4_2_ApplyingA2DTexture::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glDepthFunc(GL_LESS);    // 가까운 픽셀을 렌더링

    // VAO와 VBO 설정 (기존 코드 유지)
    generateTorus(torusPositions, torusNormals, 1.0f, 0.5f, 40, 40);

    VAO.create();
    VAO.bind();

    // 정점 위치 VBO
    pVBO.create();
    pVBO.bind();
    pVBO.allocate(torusPositions.data(), torusPositions.size() * sizeof(float));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0); // 위치 -> attribute location 0
    pVBO.release();

    // 법선 벡터 VBO
    nVBO.create();
    nVBO.bind();
    nVBO.allocate(torusNormals.data(), torusNormals.size() * sizeof(float));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1); // 법선 -> attribute location 1
    nVBO.release();

    // 텍스처 좌표 VBO 설정
    texVBO.create();
    texVBO.bind();
    texVBO.allocate(torusTexCoords.data(), torusTexCoords.size() * sizeof(float));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2); // 텍스처 좌표 -> attribute location 2
    texVBO.release();

    VAO.release();

    // 셰이더 프로그램 컴파일 및 링크
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vertShaderCode = loadShaderAsString("C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Shaders/vertShader4_2.vert");
    glShaderSource(vertShader, 1, &vertShaderCode, nullptr);
    glCompileShader(vertShader);

    const GLchar* fragShaderCode = loadShaderAsString("C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Shaders/fragShader4_2.frag");
    glShaderSource(fragShader, 1, &fragShaderCode, nullptr);
    glCompileShader(fragShader);

    programHandle = glCreateProgram();
    glAttachShader(programHandle, vertShader);
    glAttachShader(programHandle, fragShader);
    glLinkProgram(programHandle);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    glUseProgram(programHandle); // 셰이더 활성화

    // 유니폼 변수 설정
    QVector4D lightPosition(20.0f, 10.0f, -4.0f, 1.0f); // 광원 위치
    QVector3D kd(0.8f, 0.4f, 0.3f);                 // 디퓨즈 반사 계수
    QVector3D ka(0.35f, 0.2f, 0.1f);                 // 주변광 반사 계수
    QVector3D ks(0.9f, 0.9f, 0.9f);                 // 스펙큘러 반사 계수
    float shininess = 32.0f;                        // 스펙큘러 강도

    QVector3D lightIntensity(0.99f, 0.99f, 0.99f);    // 광원의 강도
    //lightIntensity*=100;
    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.translate(0.0f,-1.0f, -7.0f); // 카메라 위치 설정
    modelViewMatrix.rotate(-60.0f, 1.0f, 0.0f, 0.0f); // x축 기준 -35도 회전
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, float(width()) / height(), 0.1f, 100.0f);

    QMatrix4x4 mvp = projectionMatrix * modelViewMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // OpenGL에 유니폼 변수 전달
    glUniform4fv(glGetUniformLocation(programHandle, "Light.Position"), 1, &lightPosition[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Light.Intensity"), 1, &lightIntensity[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Ka"), 1, &ka[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Kd"), 1, &kd[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Ks"), 1, &ks[0]);
    glUniform1f(glGetUniformLocation(programHandle, "Material.Shininess"), shininess);
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "ModelViewMatrix"), 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "MVP"), 1, GL_FALSE, mvp.constData());
    glUniformMatrix3fv(glGetUniformLocation(programHandle, "NormalMatrix"), 1, GL_FALSE, normalMatrix.constData());

    // 텍스처 로드 및 바인딩
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    GLuint textureID = LoadTexture("C:/Users/user/Desktop/Qt_OpenGL/JongMin/QtOpenGL_Jongmin/Textures/brick1.jpg", programHandle, "Tex1", f);
    if (textureID == 0) {
        qDebug() << "Failed to load texture!";
    }
}

void ch4_2_ApplyingA2DTexture::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programHandle);

    VAO.bind();

    int numSides = 40;
    int numRings = 40;
    int verticesPerRing = (numSides + 1) * 2;

    for (int ring = 0; ring < numRings; ++ring)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, ring * verticesPerRing, verticesPerRing);
    }

    VAO.release();
}

void ch4_2_ApplyingA2DTexture::generateTorus(std::vector<float>& positions, std::vector<float>& normals, float outerRadius, float innerRadius, int numSides, int numRings)
{
    positions.clear();
    normals.clear();
    torusTexCoords.clear(); // 텍스처 좌표 초기화

    float ringStep = 2.0f * M_PI / numRings;
    float sideStep = 2.0f * M_PI / numSides;

    for (int ring = 0; ring <= numRings; ++ring) {
        float ringAngle = ring * ringStep;

        for (int side = 0; side <= numSides; ++side) {
            float sideAngle = side * sideStep;

            // 정점 좌표 계산
            float x = (outerRadius + innerRadius * std::cos(sideAngle)) * std::cos(ringAngle);
            float y = (outerRadius + innerRadius * std::cos(sideAngle)) * std::sin(ringAngle);
            float z = innerRadius * std::sin(sideAngle);

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);

            // 법선 계산
            float nx = std::cos(sideAngle) * std::cos(ringAngle);
            float ny = std::cos(sideAngle) * std::sin(ringAngle);
            float nz = std::sin(sideAngle);

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // 텍스처 좌표 계산
            float u = static_cast<float>(side) / numSides;
            float v = static_cast<float>(ring) / numRings;

            torusTexCoords.push_back(u);
            torusTexCoords.push_back(v);
        }
    }
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

GLuint ch4_2_ApplyingA2DTexture::LoadTexture(const char* texName, GLuint programHandle, const char* uniformName, QOpenGLFunctions* f) {
    // 텍스처 로드 및 OpenGL에 업로드
    QImage image(texName);
    if (image.isNull()) {
        qDebug() << "Error: Unable to load texture " << texName;
        return 0;
    }

    QImage formattedImage = image.convertToFormat(QImage::Format_RGBA8888);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedImage.width(), formattedImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLint uniformLocation = f->glGetUniformLocation(programHandle, uniformName);
    if (uniformLocation >= 0) {
        f->glUniform1i(uniformLocation, 0);
    } else {
        qDebug() << "Warning: Uniform variable " << uniformName << " not found in the shader program.";
    }

    return textureID;
}
