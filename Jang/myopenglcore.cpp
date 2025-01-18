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

MyOpenGLCore::MyOpenGLCore(const QString &objFilePath) {
    if (!parseObjFile(objFilePath)) {
        qDebug() << "Failed to load OBJ file in constructor.";
    }
}

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

    // 깊이 테스트 활성화
    glEnable(GL_DEPTH_TEST);
    // 깊이 비교 함수 설정 (기본값은 GL_LESS)
    glDepthFunc(GL_LESS);
    // 기타 초기화 작업
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 배경 색상 설정

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

    // m_texCoords = {
    //     0.0f, 0.0f, // 왼쪽 아래
    //     1.0f, 0.0f, // 오른쪽 아래
    //     1.0f, 1.0f, // 오른쪽 위
    // };

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
    m_uniformModelViewMatrix = glGetUniformLocation(m_program, "ModelViewMatrix");
    m_uniformNormalMatrix = glGetUniformLocation(m_program, "NormalMatrix");
    m_uniformProjectionMatrix = glGetUniformLocation(m_program, "ProjectionMatrix");
    m_uniformMVP = glGetUniformLocation(m_program, "MVP");

    // Uniform 위치 확인 디버깅
    qDebug() << "Uniform Locations:";
    qDebug() << "LightPosition:" << m_uniformLightPosition;
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
    viewMatrix.translate(0.0f, 0.0f, -24.0f); // “카메라가 z=2쪽에 위치”라고 생각
    // viewMatrix.rotate(30, QVector3D(0, 1, 0)); // 카메라를 Y축으로 30도 회전
    updateCameraView(viewMatrix); // 카메라 상태를 반영


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
    QVector4D lightPositionWorld(300.0f, 300.0f, 300.0f, 1.0f); // 월드 좌표계 조명 위치
    QVector4D lightPositionView = viewMatrix * lightPositionWorld; // 모델과는 영향 없음

    // 3. 조명 관련 Uniform 전달
    GLfloat lightPosition[4] = { lightPositionView.x(), lightPositionView.y(), lightPositionView.z(), lightPositionView.w() };
    GLfloat lightIntensity[3] = { 1.2f, 0.8f, 0.3f }; // 반사광: 밝은 주황빛

    glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, lightPosition);
    glUniform3fv(glGetUniformLocation(m_program, "LightIntensity"), 1, lightIntensity);

    GLfloat ka[3] = { 0.3f, 0.2f, 0.0f }; // 주변광 반사율: 약간 어두운 주황색
    GLfloat kd[3] = { 1.0f, 0.6f, 0.2f }; // 확산 반사율: 밝은 주황색
    GLfloat ks[3] = { 1.0f, 0.7f, 0.3f }; // 스펙큘러 반사율: 주황빛 반사 강조
    GLfloat shininess = 32.0f; // 하이라이트 크기 유지

    glUniform3fv(glGetUniformLocation(m_program, "Ka"), 1, ka);
    glUniform3fv(glGetUniformLocation(m_program, "Kd"), 1, kd);
    glUniform3fv(glGetUniformLocation(m_program, "Ks"), 1, ks);
    glUniform1f(glGetUniformLocation(m_program, "Shininess"), shininess);

    glUniformMatrix4fv(m_uniformModelViewMatrix, 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix, 1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix, 1, GL_FALSE, projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP, 1, GL_FALSE, mvpMatrix.constData());

    // 6. 도넛 렌더링
    glBindVertexArray(m_vao);
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

bool MyOpenGLCore::parseObjFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open OBJ file:" << filePath;
        return false;
    }

    QTextStream in(&file);

    // 임시 저장소
    std::vector<QVector3D> tempVertices;
    std::vector<QVector3D> tempNormals;
    std::vector<QVector2D> tempTexCoords;
    std::vector<GLuint> vertexIndices, normalIndices, texCoordIndices;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("#") || line.trimmed().isEmpty()) {
            continue; // 주석 또는 빈 줄 무시
        }

        QTextStream lineStream(&line);
        QString prefix;
        lineStream >> prefix;

        if (prefix == "v") {
            // 정점
            float x, y, z;
            lineStream >> x >> y >> z;
            tempVertices.emplace_back(x, y, z);
        } else if (prefix == "vn") {
            // 법선
            float nx, ny, nz;
            lineStream >> nx >> ny >> nz;
            tempNormals.emplace_back(nx, ny, nz);
        } else if (prefix == "vt") {
            // 텍스처 좌표
            float u, v;
            lineStream >> u >> v;
            tempTexCoords.emplace_back(u, v);
        } else if (prefix == "f") {
            // 면
            QString face;
            for (int i = 0; i < 3; ++i) {
                lineStream >> face;
                QStringList indices = face.split("/");
                if (indices.size() >= 1) vertexIndices.push_back(indices[0].toUInt() - 1);
                if (indices.size() >= 2 && !indices[1].isEmpty()) texCoordIndices.push_back(indices[1].toUInt() - 1);
                if (indices.size() == 3 && !indices[2].isEmpty()) normalIndices.push_back(indices[2].toUInt() - 1);
            }
        }
    }

    // 정점 데이터 및 인덱스 변환
    m_vertices.clear();
    m_normals.clear();
    m_texCoords.clear();
    m_indices.clear();

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        // 정점
        const QVector3D &v = tempVertices[vertexIndices[i]];
        m_vertices.push_back(v.x());
        m_vertices.push_back(v.y());
        m_vertices.push_back(v.z());

        // 텍스처 좌표
        if (!texCoordIndices.empty()) {
            const QVector2D &t = tempTexCoords[texCoordIndices[i]];
            m_texCoords.push_back(t.x());
            m_texCoords.push_back(t.y());
        }

        // 법선
        if (!normalIndices.empty()) {
            const QVector3D &n = tempNormals[normalIndices[i]];
            m_normals.push_back(n.x());
            m_normals.push_back(n.y());
            m_normals.push_back(n.z());
        }

        // 인덱스
        m_indices.push_back(static_cast<GLuint>(i));
    }

    if (!m_vertices.empty()) {
        centerObjectAtOrigin();
    }

    if (m_normals.empty()) {
        qDebug() << "Normals not found in OBJ file. Generating normals...";
        generateNormals(m_vertices, m_indices, m_normals);
    }

    qDebug() << "OBJ file loaded successfully!";
    qDebug() << "Vertices:" << m_vertices.size() / 3;
    qDebug() << "Normals:" << m_normals.size() / 3;
    qDebug() << "Texture Coords:" << m_texCoords.size() / 2;
    qDebug() << "Indices:" << m_indices.size();

    return true;
}

// 법선 생성 함수
void MyOpenGLCore::generateNormals(const std::vector<GLfloat> &vertices,
                     const std::vector<GLuint> &indices,
                     std::vector<GLfloat> &normals) {
    normals.clear();
    normals.resize(vertices.size(), 0.0f); // 정점 개수만큼 초기화 (x, y, z)

    // 각 면의 법선 계산
    for (size_t i = 0; i < indices.size(); i += 3) {
        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        QVector3D v0(vertices[idx0 * 3], vertices[idx0 * 3 + 1], vertices[idx0 * 3 + 2]);
        QVector3D v1(vertices[idx1 * 3], vertices[idx1 * 3 + 1], vertices[idx1 * 3 + 2]);
        QVector3D v2(vertices[idx2 * 3], vertices[idx2 * 3 + 1], vertices[idx2 * 3 + 2]);

        QVector3D edge1 = v1 - v0;
        QVector3D edge2 = v2 - v0;
        QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2).normalized();

        // 각 정점에 면의 법선 추가
        normals[idx0 * 3]     += faceNormal.x();
        normals[idx0 * 3 + 1] += faceNormal.y();
        normals[idx0 * 3 + 2] += faceNormal.z();

        normals[idx1 * 3]     += faceNormal.x();
        normals[idx1 * 3 + 1] += faceNormal.y();
        normals[idx1 * 3 + 2] += faceNormal.z();

        normals[idx2 * 3]     += faceNormal.x();
        normals[idx2 * 3 + 1] += faceNormal.y();
        normals[idx2 * 3 + 2] += faceNormal.z();
    }

    // 정점 법선 정규화
    for (size_t i = 0; i < normals.size(); i += 3) {
        QVector3D normal(normals[i], normals[i + 1], normals[i + 2]);
        normal.normalize();
        normals[i]     = normal.x();
        normals[i + 1] = normal.y();
        normals[i + 2] = normal.z();
    }
}

void MyOpenGLCore::centerObjectAtOrigin() {
    if (m_vertices.empty()) {
        qDebug() << "No vertices to center.";
        return;
    }

    QVector3D center(0.0f, 0.0f, 0.0f);

    // 모든 정점의 평균값 계산
    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        center += QVector3D(m_vertices[i], m_vertices[i + 1], m_vertices[i + 2]);
    }
    center /= (m_vertices.size() / 3.0f);

    // 중심점을 원점으로 이동
    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        m_vertices[i]     -= center.x();
        m_vertices[i + 1] -= center.y();
        m_vertices[i + 2] -= center.z();
    }

    qDebug() << "Object centered at origin. Center offset was:" << center;
}

void MyOpenGLCore::updateCameraView(QMatrix4x4 &viewMatrix)
{
    viewMatrix.rotate( m_cameraRotation.conjugated() );
    viewMatrix.translate( -m_cameraPosition );
    // viewMatrix.translate(-m_cameraPosition); // 카메라 위치 적용
    // viewMatrix.rotate(m_cameraRotation);    // 카메라 회전 적용
}

void MyOpenGLCore::handleKeyPressEvent(QKeyEvent *event)
{
    QVector3D forward = m_cameraRotation.rotatedVector(QVector3D(0.0f, 0.0f, -1.0f)); // 전방
    QVector3D right = m_cameraRotation.rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));    // 오른쪽
    QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);                                       // 상방 (고정)

    if (event->key() == Qt::Key_W) {
        m_cameraPosition += forward * m_moveSpeed; // 현재 방향 기준 앞으로 이동
    } else if (event->key() == Qt::Key_S) {
        m_cameraPosition -= forward * m_moveSpeed; // 현재 방향 기준 뒤로 이동
    } else if (event->key() == Qt::Key_A) {
        m_cameraPosition -= right * m_moveSpeed;   // 현재 방향 기준 왼쪽 이동
    } else if (event->key() == Qt::Key_D) {
        m_cameraPosition += right * m_moveSpeed;  // 현재 방향 기준 오른쪽 이동
    }

    qDebug() << m_cameraPosition;
}

void MyOpenGLCore::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isMousePressed = true;               // 마우스 클릭 상태 활성화
        m_lastMousePosition = event->pos();   // 마우스 클릭 위치 저장
    }
}

void MyOpenGLCore::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isMousePressed = false; // 마우스 클릭 상태 비활성화
    }
}

void MyOpenGLCore::handleMouseMoveEvent(QMouseEvent *event)
{
    if (!m_isMousePressed) {
        return; // 마우스를 클릭하고 있지 않으면 회전하지 않음
    }

    QPoint delta = event->pos() - m_lastMousePosition; // 마우스 이동 거리 계산
    m_lastMousePosition = event->pos();               // 현재 마우스 위치 저장

    // 마우스 이동에 따른 회전 각도 계산
    float yaw = -delta.x() * m_rotationSpeed;   // 좌우 이동은 yaw
    float pitch = -delta.y() * m_rotationSpeed; // 상하 이동은 pitch (음수로 뒤집음)

    // 쿼터니언을 사용한 회전 적용
    QQuaternion yawRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yaw);  // Y축 회전
    QQuaternion pitchRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, pitch); // X축 회전
    m_cameraRotation = yawRotation * m_cameraRotation * pitchRotation; // 누적 회전
}


