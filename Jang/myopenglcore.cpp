#include "myopenglcore.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QMatrix4x4>
#include <QtMath>

// 생성자
MyOpenGLCore::MyOpenGLCore(const std::vector<GLfloat> &vertices,
                           const std::vector<GLuint> &indices,
                           const std::vector<GLfloat> &normals)
    : m_vertices(vertices),
    m_indices(indices),
    m_normals(normals)
 {}


MyOpenGLCore::~MyOpenGLCore()
{
    glDeleteProgram(m_program); // 셰이더 프로그램 삭제
    glDeleteBuffers(1, &m_vbo); // VBO 삭제
    glDeleteBuffers(1, &m_cbo); // CBO 삭제
    glDeleteVertexArrays(1, &m_vao); // VAO 삭제
}

void MyOpenGLCore::initialize()
{
    checkDefaults();

    initializeOpenGLFunctions(); // OpenGL 함수 초기화
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);      // 뒷면 제거
    glFrontFace(GL_CCW);      // 반시계 방향을 앞면으로 설정 (기본값)

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO 생성 및 데이터 전송 (정점)
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // NBO 생성 및 데이터 전송 (법선)
    glGenBuffers(1, &m_nbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(GLfloat), m_normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // // CBO 생성 및 데이터 전송 (색상)
    // glGenBuffers(1, &m_cbo);
    // glBindBuffer(GL_ARRAY_BUFFER, m_cbo);
    // glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof(GLfloat), m_colors.data(), GL_STATIC_DRAW);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // glEnableVertexAttribArray(2);

    // // TBO 생성 및 데이터 전송 (텍스처 좌표)
    // glGenBuffers(1, &m_tbo);
    // glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
    // glBufferData(GL_ARRAY_BUFFER, m_texCoords.size() * sizeof(GLfloat), m_texCoords.data(), GL_STATIC_DRAW);
    // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    // glEnableVertexAttribArray(3);

    // EBO 생성 및 데이터 전송 (인덱스)
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    // VAO 해제
    glBindVertexArray(0);
}

bool MyOpenGLCore::loadShaders(const QString &vertexShaderPath, const QString &fragmentShaderPath)
{
    // 셰이더 파일 읽기
    auto readShaderFile = [](const QString &path) -> QString {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Failed to open shader file: " << path.toStdString() << std::endl;
            return "";
        }
        QTextStream in(&file);
        return in.readAll();
    };

    QString vertexShaderSource = readShaderFile(vertexShaderPath);
    QString fragmentShaderSource = readShaderFile(fragmentShaderPath);

    if (vertexShaderSource.isEmpty() || fragmentShaderSource.isEmpty())
        return false;

    // 정점 셰이더 생성 및 컴파일
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    QByteArray vertexShaderData = vertexShaderSource.toUtf8();
    const char *vertexShaderCode = vertexShaderData.constData();
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader);

    // 프래그먼트 셰이더 생성 및 컴파일
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    QByteArray fragmentShaderData = fragmentShaderSource.toUtf8();
    const char *fragmentShaderCode = fragmentShaderData.constData();
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader);

    // 셰이더 프로그램 생성 및 링크
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);
    checkProgramLinkStatus(m_program);

    // Uniform 변수 로드
    m_uniformLightPosition = glGetUniformLocation(m_program, "LightPosition");
    m_uniformKd = glGetUniformLocation(m_program, "Kd");
    m_uniformLd = glGetUniformLocation(m_program, "Light.Ld");
    m_uniformModelViewMatrix = glGetUniformLocation(m_program, "ModelViewMatrix");
    m_uniformNormalMatrix = glGetUniformLocation(m_program, "NormalMatrix");
    m_uniformProjectionMatrix = glGetUniformLocation(m_program, "ProjectionMatrix");
    m_uniformMVP = glGetUniformLocation(m_program, "MVP");

    // Uniform 위치 확인 디버깅
    qDebug() << "Uniform Locations:";
    qDebug() << "LightPosition:" << m_uniformLightPosition;
    qDebug() << "Kd:" << m_uniformKd;
    qDebug() << "Light.Ld:" << m_uniformLd;
    qDebug() << "ModelViewMatrix:" << m_uniformModelViewMatrix;
    qDebug() << "NormalMatrix:" << m_uniformNormalMatrix;
    qDebug() << "ProjectionMatrix:" << m_uniformProjectionMatrix;
    qDebug() << "MVP:" << m_uniformMVP;

    // 셰이더 객체 삭제
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void MyOpenGLCore::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);

    // 1. 행렬 정의
    QMatrix4x4 modelMatrix, viewMatrix, projectionMatrix;

    // 1) 모델 변환 (도넛을 월드좌표 안에서 어떻게 회전/이동할지)
    modelMatrix.setToIdentity();
    // modelMatrix.rotate(30, QVector3D(0, 1, 0)); // 도넛 자체를 y축으로 30도 회전

    // 2) 뷰 변환 (카메라가 월드에서 어디 위치해있는가)
    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -2.0f); // “카메라가 z=2쪽에 위치”라고 생각
    viewMatrix.rotate(30, QVector3D(0, 1, 0)); // 카메라를 Y축으로 30도 회전

    // 3) 최종 모델뷰
    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;

    // 4) 노멀 매트릭스
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // 5) 투영 행렬
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 100.f);

    // 6) MVP
    QMatrix4x4 mvpMatrix = projectionMatrix * modelViewMatrix;

    // 2. 조명 위치를 뷰 좌표계로 변환
    QVector4D lightPositionWorld(1.0f, 1.0f, 1.0f, 1.0f); // 월드 좌표계 조명 위치
    QVector4D lightPositionView = viewMatrix * lightPositionWorld; // 모델과는 영향 없음

    // 3. 조명 관련 Uniform 전달
    GLfloat lightPosition[4] = { lightPositionView.x(), lightPositionView.y(), lightPositionView.z(), lightPositionView.w() };
    GLfloat lightLa[3] = { 0.5f, 0.5f, 0.5f };
    // GLfloat lightLa[3] = { 0.9f, 0.9f, 0.9f };
    // GLfloat lightLd[3] = { 1.0f, 1.0f, 1.0f };
    // GLfloat lightLs[3] = { 1.0f, 1.0f, 1.0f };
    GLfloat lightLd[3] = { 1.2f, 0.8f, 0.3f }; // 확산광: 밝은 주황빛
    GLfloat lightLs[3] = { 1.2f, 0.8f, 0.3f }; // 반사광: 밝은 주황빛

    glUniform4fv(glGetUniformLocation(m_program, "Light.Position"), 1, lightPosition);
    glUniform3fv(glGetUniformLocation(m_program, "Light.La"), 1, lightLa);
    glUniform3fv(glGetUniformLocation(m_program, "Light.Ld"), 1, lightLd);
    glUniform3fv(glGetUniformLocation(m_program, "Light.Ls"), 1, lightLs);

    // GLfloat materialKa[3] = { 0.3f, 0.3f, 0.3f };
    // GLfloat materialKd[3] = { 0.9f, 0.5f, 0.3f };
    // GLfloat materialKs[3] = { 0.5f, 0.5f, 0.5f };
    // GLfloat shininess = 32.0f;
    GLfloat materialKa[3] = { 0.3f, 0.2f, 0.0f }; // 주변광 반사율: 약간 어두운 주황색
    GLfloat materialKd[3] = { 1.0f, 0.6f, 0.2f }; // 확산 반사율: 밝은 주황색
    GLfloat materialKs[3] = { 1.0f, 0.7f, 0.3f }; // 스펙큘러 반사율: 주황빛 반사 강조
    GLfloat shininess = 32.0f; // 하이라이트 크기 유지

    glUniform3fv(glGetUniformLocation(m_program, "Material.Ka"), 1, materialKa);
    glUniform3fv(glGetUniformLocation(m_program, "Material.Kd"), 1, materialKd);
    glUniform3fv(glGetUniformLocation(m_program, "Material.Ks"), 1, materialKs);
    glUniform1f(glGetUniformLocation(m_program, "Material.Shininess"), shininess);

    glUniformMatrix4fv(m_uniformModelViewMatrix, 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix, 1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix, 1, GL_FALSE, projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP, 1, GL_FALSE, mvpMatrix.constData());

    // 6. 도넛 렌더링
    glBindVertexArray(m_vao);

    GLuint adsIndex = glGetSubroutineIndex( m_program, GL_VERTEX_SHADER,"phongModel" );
    GLuint diffuseIndex =glGetSubroutineIndex( m_program, GL_VERTEX_SHADER, "diffuseOnly");
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &adsIndex);
    // Render the left teapot
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &diffuseIndex);

    // Render the right teapot
    modelMatrix.translate(0.5,0,0);
    modelViewMatrix = viewMatrix * modelMatrix;
    normalMatrix = modelViewMatrix.normalMatrix();
    mvpMatrix = projectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(m_uniformModelViewMatrix, 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix, 1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix, 1, GL_FALSE, projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP, 1, GL_FALSE, mvpMatrix.constData());
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


void MyOpenGLCore::checkShaderCompileStatus(GLuint shader)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader Compilation Failed\n" << infoLog << std::endl;
    }
}

void MyOpenGLCore::checkProgramLinkStatus(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR: Program Linking Failed\n" << infoLog << std::endl;
    }
}

void MyOpenGLCore::activeVertexInputAttributesAndIndices()
{
    GLint maxLength, nAttribs;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &nAttribs);
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
    GLchar *name = (GLchar *)malloc(maxLength);

    GLint written, size, location;
    GLenum type;
    qDebug() << " Index | Name";
    qDebug() << "------------------------------------------------";
    for (int i = 0; i < nAttribs; i++) {
        glGetActiveAttrib(m_program, i, maxLength, &written, &size, &type, name);
        location = glGetAttribLocation(m_program, name);
        qDebug().noquote() << QString::asprintf(" %-5d | %s", location, name);
    }
    free(name);
}

void MyOpenGLCore::activeUniformVariables()
{
    GLint nUniforms, maxLen;
    glGetProgramiv( m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH,
                   &maxLen);
    glGetProgramiv( m_program, GL_ACTIVE_UNIFORMS,
                   &nUniforms);

    GLchar * name = (GLchar *) malloc( maxLen );
    GLint size, location;
    GLsizei written;
    GLenum type;
    qDebug() << (" Location | Name");
    qDebug() << ("------------------------------------------------");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( m_program, i, maxLen, &written,
                           &size, &type, name );
        location = glGetUniformLocation(m_program, name);
        qDebug().noquote() << QString::asprintf(" %-5d | %s", location, name);
    }
    free(name);
}

// 기본값 확인 함수
void MyOpenGLCore::checkDefaults() {
    if (m_colors == defaultColors) {
        std::cout << "[INFO] Colors are using default values (White)." << std::endl;
    } else {
        std::cout << "[INFO] Colors are custom-defined." << std::endl;
    }

    if (m_texCoords == defaultTexCoords) {
        std::cout << "[INFO] Texture coordinates are using default values." << std::endl;
    } else {
        std::cout << "[INFO] Texture coordinates are custom-defined." << std::endl;
    }
}
