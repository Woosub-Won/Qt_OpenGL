#include "ch2_3ImplementingPerVertex.h"

#include <QFile>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <vector>
#include <cmath>

void ch2_3ImplementingPerVertex::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glDepthFunc(GL_LESS);    // 가까운 픽셀을 렌더링

    // VAO와 VBO 설정 (기존 코드 유지)
    generateTorus(torusPositions, torusNormals, 1.0f, 0.5f, 40, 40); // 그냥 gpt가 도와줌 ㅋ

    VAO.create();
    VAO.bind();

    pVBO.create();
    pVBO.bind();
    pVBO.allocate(torusPositions.data(), torusPositions.size() * sizeof(float));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    pVBO.release();

    nVBO.create();
    nVBO.bind();
    nVBO.allocate(torusNormals.data(), torusNormals.size() * sizeof(float));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    nVBO.release();

    VAO.release();

    // 셰이더 프로그램 컴파일 및 링크
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vertShaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/vertShader2_3.vert");
    glShaderSource(vertShader, 1, &vertShaderCode, nullptr);
    glCompileShader(vertShader);

    const GLchar* fragShaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/fragShader2_3.frag");
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
    QVector4D lightPosition(20.0f, 10.0f, -4.0f, 1.0f);
    //QVector4D lightPosition(5.0f, 5.0f, -4.0f, 1.0f); // 광원 위치
    QVector3D kd(0.5f, 0.5f, 0.5f);                 // 디퓨즈 반사 계수 (빨간색)
    QVector3D ld(0.5f, 0.5f, 0.5f);                 // 광원의 강도 (흰색)

    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.translate(0.0f, -1.0f, -7.0f); // 카메라 위치 설정
    modelViewMatrix.rotate(-60.0f, 1.0f, 0.0f, 0.0f); // x축 기준 -35도 회전
    modelViewMatrix.rotate(0.0f, 0.0f, 1.0f, 0.0f);  // y축 기준 30도 회전

    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, float(width()) / height(), 0.1f, 100.0f);

    QMatrix4x4 mvp = projectionMatrix * modelViewMatrix;

    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // LightInfo 구조체 설정
    QVector3D lightAmbient(1.0f, 1.0f, 1.0f);
    QVector3D lightDiffuse(1.0f, 1.0f, 1.0f);
    QVector3D lightSpecular(1.0f, 1.0f, 1.0f);

    glUniform4fv(glGetUniformLocation(programHandle, "Light.Position"), 1, &lightPosition[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Light.La"), 1, &lightAmbient[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Light.Ld"), 1, &lightDiffuse[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Light.Ls"), 1, &lightSpecular[0]);

    // MaterialInfo 구조체 설정
    QVector3D materialAmbient(0.6f, 0.3f, 0.2f);
    QVector3D materialDiffuse(0.7f, 0.4f, 0.2f);
    QVector3D materialSpecular(1.0f, 1.0f, 1.0f);
    float materialShininess = 150.0f;
    materialAmbient *= 0.8f;
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Ka"), 1, &materialAmbient[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Kd"), 1, &materialDiffuse[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Material.Ks"), 1, &materialSpecular[0]);
    glUniform1f(glGetUniformLocation(programHandle, "Material.Shininess"), materialShininess);

    // OpenGL에 유니폼 변수 전달
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "ModelViewMatrix"), 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "MVP"), 1, GL_FALSE, mvp.constData());
    glUniformMatrix3fv(glGetUniformLocation(programHandle, "NormalMatrix"), 1, GL_FALSE, normalMatrix.constData());
    glUniform3fv(glGetUniformLocation(programHandle, "Kd"), 1, &kd[0]);
    glUniform3fv(glGetUniformLocation(programHandle, "Ld"), 1, &ld[0]);
}

void ch2_3ImplementingPerVertex::paintGL()
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

void ch2_3ImplementingPerVertex::generateTorus(std::vector<float> &positions, std::vector<float> &normals, float outerRadius, float innerRadius, int numSides, int numRings)
{
    positions.clear();
    normals.clear();

    float ringStep = 2.0f * M_PI / numRings;
    float sideStep = 2.0f * M_PI / numSides;

    for (int ring = 0; ring <= numRings; ++ring)
    {
        float ringAngle = ring * ringStep;
        float cosRing = std::cos(ringAngle);
        float sinRing = std::sin(ringAngle);

        for (int side = 0; side <= numSides; ++side)
        {
            float sideAngle = side * sideStep;
            float cosSide = std::cos(sideAngle);
            float sinSide = std::sin(sideAngle);

            // 첫 번째 정점
            float x1 = (outerRadius + innerRadius * cosSide) * cosRing;
            float y1 = (outerRadius + innerRadius * cosSide) * sinRing;
            float z1 = innerRadius * sinSide;

            positions.push_back(x1);
            positions.push_back(y1);
            positions.push_back(z1);

            float nx1 = cosSide * cosRing;
            float ny1 = cosSide * sinRing;
            float nz1 = sinSide;

            normals.push_back(nx1);
            normals.push_back(ny1);
            normals.push_back(nz1);

            // 두 번째 정점 (다음 링)
            float nextRingAngle = (ring + 1) * ringStep;
            float cosNextRing = std::cos(nextRingAngle);
            float sinNextRing = std::sin(nextRingAngle);

            float x2 = (outerRadius + innerRadius * cosSide) * cosNextRing;
            float y2 = (outerRadius + innerRadius * cosSide) * sinNextRing;
            float z2 = innerRadius * sinSide;

            positions.push_back(x2);
            positions.push_back(y2);
            positions.push_back(z2);

            float nx2 = cosSide * cosNextRing;
            float ny2 = cosSide * sinNextRing;
            float nz2 = sinSide;

            normals.push_back(nx2);
            normals.push_back(ny2);
            normals.push_back(nz2);
        }
    }
}

const char *ch2_3ImplementingPerVertex::loadShaderAsString(const QString &filePath)
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
