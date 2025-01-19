#include "myopenglcore.h"
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <iostream>

//--------------------------------------
// 생성자 / 소멸자
//--------------------------------------
MyOpenGLCore::MyOpenGLCore(const QString &objFilePath,
                           const QString &textureFilePath)
    : m_program(0),
    m_vao(0),
    m_vbo(0),
    m_nbo(0),
    m_cbo(0),
    m_tbo(0),
    m_ebo(0),
    m_texture(0),
    m_objFilePath(objFilePath),
    m_textureFilePath(textureFilePath),
    m_cameraPosition(0.0f, 0.0f, 5.0f),
    m_cameraRotation(),
    m_isMousePressed(false),
    m_moveSpeed(0.1f),
    m_rotationSpeed(0.2f),
    m_uniformLightPosition(-1),
    m_uniformModelViewMatrix(-1),
    m_uniformNormalMatrix(-1),
    m_uniformProjectionMatrix(-1),
    m_uniformMVP(-1)
{
    qDebug() << "[MyOpenGLCore] Constructor called. "
             << "OBJ path:" << m_objFilePath
             << "Texture path:" << m_textureFilePath;
}

MyOpenGLCore::~MyOpenGLCore()
{
    qDebug() << "[MyOpenGLCore] Destructor called.";
    glDeleteProgram(m_program);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_nbo);
    glDeleteBuffers(1, &m_cbo);
    glDeleteBuffers(1, &m_tbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

//--------------------------------------
// 초기화(메인 루프 시작 전 호출)
//--------------------------------------
void MyOpenGLCore::initialize(const QString &vertexShaderPath,
                              const QString &fragmentShaderPath)
{
    qDebug() << "[MyOpenGLCore::initialize] Start";

    // 1. OpenGL 함수 초기화
    initializeOpenGLFunctions();
    qDebug() << "OpenGL Functions initialized.";

    // 2. 모델 / 텍스처 로드
    loadModel();
    loadTexture();
    loadTexture2();

    // 3. 셰이더 로드/컴파일/링크
    loadShaders(vertexShaderPath, fragmentShaderPath);

    // 4. VAO, VBO 등 버퍼 생성
    createBuffers();

    // 5. 텍스처 설정
    configureTexture();
    configureTexture2();

    // 6. 기본 OpenGL 상태 설정
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    qDebug() << "[MyOpenGLCore::initialize] Done";
}

//--------------------------------------
// 2-1. 모델 로드
//--------------------------------------
void MyOpenGLCore::loadModel()
{
    // OBJ 경로가 있다면 OBJ 로드
    // 없다면, 혹은 실패하면 임시 기본 geometry 구성 등
    if (!m_objFilePath.isEmpty()) {
        qDebug() << "[MyOpenGLCore::loadModel] Loading OBJ from" << m_objFilePath;
        if (!parseObjFile(m_objFilePath)) {
            qWarning() << "Failed to load OBJ file. Fallback to default geometry?";
            // 필요 시 기본 geometry 등
        }
    } else {
        qDebug() << "[MyOpenGLCore::loadModel] No OBJ path given. Using default geometry?";

        // 예시: 삼각형 하나만 생성
        // 실제로 원하는 기본 geometry를 채워넣을 수 있음
        m_vertices = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f
        };
        m_indices = { 0, 1, 2 };
        // 기본 법선 생성
        generateNormals(m_vertices, m_indices, m_normals);
        // 기본 텍스처 좌표
        m_texCoords = { 0.0f, 0.0f,
                       1.0f, 0.0f,
                       0.5f, 1.0f };
    }

    qDebug() << "[MyOpenGLCore::loadModel] Vertices:" << m_vertices.size() / 3
             << "Normals:" << m_normals.size() / 3
             << "TexCoords:" << m_texCoords.size() / 2
             << "Indices:" << m_indices.size();
}

//--------------------------------------
// 2-2. 텍스처 로드
//--------------------------------------
void MyOpenGLCore::loadTexture()
{
    if (!m_textureFilePath.isEmpty()) {
        qDebug() << "[MyOpenGLCore::loadTexture] Loading texture from" << m_textureFilePath;
        if (!m_textureImage.load(m_textureFilePath)) {
            qWarning() << "Failed to load texture image from:" << m_textureFilePath;
        } else {
            qDebug() << "Texture loaded. Size:" << m_textureImage.width()
            << "x" << m_textureImage.height();
        }
    } else {
        qDebug() << "[MyOpenGLCore::loadTexture] No texture path given. Using default black image?";
        m_textureImage = QImage(64, 64, QImage::Format_RGBA8888);
        m_textureImage.fill(Qt::black);
    }
}
void MyOpenGLCore::loadTexture2()
{
    m_textureFilePath2 = "../../dry-rocky-ground_normal-ogl.png";
    if (!m_textureFilePath2.isEmpty()) {
        qDebug() << "[MyOpenGLCore::loadTexture] Loading texture from" << m_textureFilePath2;
        if (!m_textureImage2.load(m_textureFilePath2)) {
            qWarning() << "Failed to load texture image from:" << m_textureFilePath2;
        } else {
            qDebug() << "Texture loaded. Size:" << m_textureImage2.width()
            << "x" << m_textureImage2.height();
        }
    } else {
        qDebug() << "[MyOpenGLCore::loadTexture] No texture path given. Using default black image?";
        m_textureImage2 = QImage(64, 64, QImage::Format_RGBA8888);
        m_textureImage2.fill(Qt::black);
    }
}

//--------------------------------------
// 3. 셰이더 로드 및 프로그램 링크
//--------------------------------------
void MyOpenGLCore::loadShaders(const QString &vertexShaderPath,
                               const QString &fragmentShaderPath)
{
    // 파일에서 셰이더 소스 읽기
    auto readShaderFile = [&](const QString &path) -> QByteArray {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open shader file:" << path;
            return QByteArray();
        }
        return file.readAll();
    };

    QByteArray vertSource = readShaderFile(vertexShaderPath);
    QByteArray fragSource = readShaderFile(fragmentShaderPath);
    if (vertSource.isEmpty() || fragSource.isEmpty()) {
        qWarning() << "Shader source is empty. Check file path or contents!";
        return;
    }

    // 셰이더 객체 생성
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 컴파일
    if (!compileShader(vertexShader, vertSource.constData()) ||
        !compileShader(fragmentShader, fragSource.constData())) {
        qWarning() << "Shader compilation failed!";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // 프로그램 생성 및 링크
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);

    if (!linkProgram(m_program)) {
        qWarning() << "Program linking failed!";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // 필요 없어진 셰이더 객체 삭제
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Uniform 변수 로드
    m_uniformLightPosition      = glGetUniformLocation(m_program, "LightPosition");
    m_uniformModelViewMatrix    = glGetUniformLocation(m_program, "ModelViewMatrix");
    m_uniformNormalMatrix       = glGetUniformLocation(m_program, "NormalMatrix");
    m_uniformProjectionMatrix   = glGetUniformLocation(m_program, "ProjectionMatrix");
    m_uniformMVP                = glGetUniformLocation(m_program, "MVP");

    qDebug() << "[MyOpenGLCore::loadShaders] Uniform Locations:"
             << "\n LightPosition:" << m_uniformLightPosition
             << "\n ModelViewMatrix:" << m_uniformModelViewMatrix
             << "\n NormalMatrix:" << m_uniformNormalMatrix
             << "\n ProjectionMatrix:" << m_uniformProjectionMatrix
             << "\n MVP:" << m_uniformMVP;
}

//--------------------------------------
// 3-1. 셰이더 컴파일
//--------------------------------------
bool MyOpenGLCore::compileShader(GLuint shader, const char* source)
{
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // 컴파일 에러 확인
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        qWarning() << "Shader compilation failed:\n" << infoLog;
        return false;
    }
    return true;
}

//--------------------------------------
// 3-2. 프로그램 링크
//--------------------------------------
bool MyOpenGLCore::linkProgram(GLuint program)
{
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        qWarning() << "Program linking failed:\n" << infoLog;
        return false;
    }
    return true;
}

//--------------------------------------
// 4. 버퍼 및 VAO 생성/설정
//--------------------------------------
void MyOpenGLCore::createBuffers()
{
    qDebug() << "[MyOpenGLCore::createBuffers] Start";

    // VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO: 정점
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices.size() * sizeof(GLfloat),
                 m_vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // NBO: 법선
    glGenBuffers(1, &m_nbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
    glBufferData(GL_ARRAY_BUFFER,
                 m_normals.size() * sizeof(GLfloat),
                 m_normals.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // TBO: 텍스처 좌표
    glGenBuffers(1, &m_tbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
    glBufferData(GL_ARRAY_BUFFER,
                 m_texCoords.size() * sizeof(GLfloat),
                 m_texCoords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    // Tangent VBO (4 floats per vertex)
    if (!m_tangents.empty()) {
        glGenBuffers(1, &m_tangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_tangentBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     m_tangents.size() * sizeof(GLfloat),
                     m_tangents.data(),
                     GL_STATIC_DRAW);
        // layout(location = 3) in vec4 VertexTangent;
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(3);
    }

    // EBO: 인덱스
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_indices.size() * sizeof(GLuint),
                 m_indices.data(),
                 GL_STATIC_DRAW);

    // VAO unbind
    glBindVertexArray(0);

    qDebug() << "[MyOpenGLCore::createBuffers] Done";
}

//--------------------------------------
// 5. 텍스처 OpenGL 설정
//--------------------------------------
void MyOpenGLCore::configureTexture()
{
    qDebug() << "[MyOpenGLCore::configureTexture] Start";

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // QImage를 RGBA8888 포맷으로 변환
    QImage glImage = m_textureImage.convertToFormat(QImage::Format_RGBA8888);

    // 텍스처 데이터 업로드
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 glImage.width(),
                 glImage.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 glImage.bits());

    // 필터링, 래핑
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    qDebug() << "[MyOpenGLCore::configureTexture] Done";
}

void MyOpenGLCore::configureTexture2()
{
    qDebug() << "[MyOpenGLCore::configureTexture] Start";

    glGenTextures(1, &m_texture2);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    // QImage를 RGBA8888 포맷으로 변환
    QImage glImage = m_textureImage2.convertToFormat(QImage::Format_RGBA8888);

    // 텍스처 데이터 업로드
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 glImage.width(),
                 glImage.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 glImage.bits());

    // 필터링, 래핑
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 1);

    qDebug() << "[MyOpenGLCore::configureTexture] Done";
}
//--------------------------------------
// 메인 렌더링 함수
//--------------------------------------
void MyOpenGLCore::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);

    // 텍스처 활성화
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // 텍스처 유니폼
    GLint texLoc = glGetUniformLocation(m_program, "ColorTex");
    if (texLoc >= 0) {
        glUniform1i(texLoc, 0);
    }

    // 텍스처 활성화
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    // 텍스처 유니폼
    GLint texLoc2 = glGetUniformLocation(m_program, "NormalMapTex");
    if (texLoc2 >= 0) {
        glUniform1i(texLoc2, 1);
    }

    // 카메라/조명/행렬 설정
    setMatrices();

    // 도형 렌더링
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

//--------------------------------------
// 조명, 행렬 관련 설정(뷰, 투영 등)
//--------------------------------------
void MyOpenGLCore::setMatrices()
{
    // 1) 모델 행렬
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    // 필요 시 회전, 스케일, 이동 적용 가능

    // 2) 뷰 행렬 (카메라)
    QMatrix4x4 viewMatrix;
    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -24.0f);
    updateCameraView(viewMatrix);

    // 3) 모델뷰
    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;

    // 4) 노멀 매트릭스
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // 5) 투영 행렬
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

    // 6) MVP
    QMatrix4x4 mvpMatrix = projectionMatrix * modelViewMatrix;

    // 조명 위치(월드->뷰)
    QVector4D lightPosWorld(300.f, 300.f, 300.f, 1.f);
    QVector4D lightPosView = viewMatrix * lightPosWorld;
    GLfloat lightPosition[4] = {
        lightPosView.x(), lightPosView.y(),
        lightPosView.z(), lightPosView.w()
    };

    // 유니폼 전달
    glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, lightPosition);

    // 머티리얼, 조명 세팅 예시
    GLfloat lightIntensity[3] = { 1.2f, 0.8f, 0.3f };
    glUniform3fv(glGetUniformLocation(m_program, "LightIntensity"), 1, lightIntensity);

    // 예시 머티리얼 값
    GLfloat ka[3] = { 0.3f, 0.2f, 0.0f };
    GLfloat kd[3] = { 1.0f, 0.6f, 0.2f };
    GLfloat ks[3] = { 1.0f, 0.7f, 0.3f };
    GLfloat shininess = 32.0f;
    glUniform3fv(glGetUniformLocation(m_program, "Ka"), 1, ka);
    glUniform3fv(glGetUniformLocation(m_program, "Kd"), 1, kd);
    glUniform3fv(glGetUniformLocation(m_program, "Ks"), 1, ks);
    glUniform1f(glGetUniformLocation(m_program, "Shininess"), shininess);

    // 행렬 유니폼 전달
    glUniformMatrix4fv(m_uniformModelViewMatrix, 1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix, 1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix, 1, GL_FALSE, projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP, 1, GL_FALSE, mvpMatrix.constData());
}

//--------------------------------------
// 카메라 변환 적용
//--------------------------------------
void MyOpenGLCore::updateCameraView(QMatrix4x4 &viewMatrix)
{
    // 회전은 쿼터니언의 역(컨주게이트) 곱 적용
    viewMatrix.rotate(m_cameraRotation.conjugated());
    viewMatrix.translate(-m_cameraPosition);
}

//--------------------------------------
// 이벤트 처리: 키보드
//--------------------------------------
void MyOpenGLCore::handleKeyPressEvent(QKeyEvent *event)
{
    QVector3D forward = m_cameraRotation.rotatedVector(QVector3D(0.f, 0.f, -1.f));
    QVector3D right   = m_cameraRotation.rotatedVector(QVector3D(1.f, 0.f, 0.f));

    if (event->key() == Qt::Key_W) {
        m_cameraPosition += forward * m_moveSpeed;
    } else if (event->key() == Qt::Key_S) {
        m_cameraPosition -= forward * m_moveSpeed;
    } else if (event->key() == Qt::Key_A) {
        m_cameraPosition -= right * m_moveSpeed;
    } else if (event->key() == Qt::Key_D) {
        m_cameraPosition += right * m_moveSpeed;
    }
    qDebug() << "[handleKeyPressEvent] CameraPosition:" << m_cameraPosition;
}

//--------------------------------------
// 이벤트 처리: 마우스
//--------------------------------------
void MyOpenGLCore::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isMousePressed = true;
        m_lastMousePosition = event->pos();
    }
}

void MyOpenGLCore::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isMousePressed = false;
    }
}

void MyOpenGLCore::handleMouseMoveEvent(QMouseEvent *event)
{
    if (!m_isMousePressed) return;

    QPoint delta = event->pos() - m_lastMousePosition;
    m_lastMousePosition = event->pos();

    float yaw   = -delta.x() * m_rotationSpeed;
    float pitch = -delta.y() * m_rotationSpeed;

    // 쿼터니언 회전
    QQuaternion yawRotation   = QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, yaw);
    QQuaternion pitchRotation = QQuaternion::fromAxisAndAngle(1.f, 0.f, 0.f, pitch);

    m_cameraRotation = yawRotation * m_cameraRotation * pitchRotation;
}

//--------------------------------------
// OBJ 파일 파싱
//--------------------------------------
bool MyOpenGLCore::parseObjFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open OBJ file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    std::vector<QVector3D> tempVertices;
    std::vector<QVector3D> tempNormals;
    std::vector<QVector2D> tempTexCoords;
    std::vector<GLuint>    vertexIndices, normalIndices, texCoordIndices;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("#") || line.isEmpty()) {
            continue;
        }

        QTextStream lineStream(&line);
        QString prefix;
        lineStream >> prefix;

        if (prefix == "v") {
            float x, y, z;
            lineStream >> x >> y >> z;
            tempVertices.emplace_back(x, y, z);
        } else if (prefix == "vn") {
            float nx, ny, nz;
            lineStream >> nx >> ny >> nz;
            tempNormals.emplace_back(nx, ny, nz);
        } else if (prefix == "vt") {
            float u, v;
            lineStream >> u >> v;
            tempTexCoords.emplace_back(u, v);
        } else if (prefix == "f") {
            for (int i = 0; i < 3; ++i) {
                QString face;
                lineStream >> face;
                QStringList indices = face.split("/");
                if (indices.size() >= 1) vertexIndices.push_back(indices[0].toUInt() - 1);
                if (indices.size() >= 2 && !indices[1].isEmpty()) {
                    texCoordIndices.push_back(indices[1].toUInt() - 1);
                }
                if (indices.size() == 3 && !indices[2].isEmpty()) {
                    normalIndices.push_back(indices[2].toUInt() - 1);
                }
            }
        }
    }

    m_vertices.clear();
    m_normals.clear();
    m_texCoords.clear();
    m_indices.clear();

    // 정점, 인덱스 재구성
    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        QVector3D v = tempVertices[vertexIndices[i]];
        m_vertices.push_back(v.x());
        m_vertices.push_back(v.y());
        m_vertices.push_back(v.z());

        if (!texCoordIndices.empty()) {
            QVector2D t = tempTexCoords[texCoordIndices[i]];
            m_texCoords.push_back(t.x());
            m_texCoords.push_back(t.y());
        }

        if (!normalIndices.empty()) {
            QVector3D n = tempNormals[normalIndices[i]];
            m_normals.push_back(n.x());
            m_normals.push_back(n.y());
            m_normals.push_back(n.z());
        }
        m_indices.push_back(static_cast<GLuint>(i));
    }

    if (!m_vertices.empty()) {
        centerObjectAtOrigin();
    }
    if (m_normals.empty()) {
        qDebug() << "Normals not found. Generating normals...";
        generateNormals(m_vertices, m_indices, m_normals);
    }

    if (!m_vertices.empty() && !m_normals.empty() && !m_texCoords.empty()) {
        qDebug() << "Generating tangents for normal mapping...";
        generateTangents(m_vertices, m_normals, m_texCoords, m_indices, m_tangents);
        qDebug() << "Tangents generated. Count:" << m_tangents.size() / 4;
    }

    qDebug() << "OBJ loaded. V:" << m_vertices.size()/3
             << "N:" << m_normals.size()/3
             << "T:" << m_texCoords.size()/2
             << "I:" << m_indices.size();

    return true;
}

//--------------------------------------
// 법선 생성
//--------------------------------------
void MyOpenGLCore::generateNormals(const std::vector<GLfloat> &vertices,
                                   const std::vector<GLuint> &indices,
                                   std::vector<GLfloat> &normals)
{
    normals.clear();
    normals.resize(vertices.size(), 0.f);

    for (size_t i = 0; i < indices.size(); i += 3) {
        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        QVector3D v0(vertices[idx0*3], vertices[idx0*3+1], vertices[idx0*3+2]);
        QVector3D v1(vertices[idx1*3], vertices[idx1*3+1], vertices[idx1*3+2]);
        QVector3D v2(vertices[idx2*3], vertices[idx2*3+1], vertices[idx2*3+2]);

        QVector3D edge1 = v1 - v0;
        QVector3D edge2 = v2 - v0;
        QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2).normalized();

        normals[idx0*3 + 0] += faceNormal.x();
        normals[idx0*3 + 1] += faceNormal.y();
        normals[idx0*3 + 2] += faceNormal.z();

        normals[idx1*3 + 0] += faceNormal.x();
        normals[idx1*3 + 1] += faceNormal.y();
        normals[idx1*3 + 2] += faceNormal.z();

        normals[idx2*3 + 0] += faceNormal.x();
        normals[idx2*3 + 1] += faceNormal.y();
        normals[idx2*3 + 2] += faceNormal.z();
    }

    for (size_t i = 0; i < normals.size(); i += 3) {
        QVector3D n(normals[i], normals[i+1], normals[i+2]);
        n.normalize();
        normals[i]   = n.x();
        normals[i+1] = n.y();
        normals[i+2] = n.z();
    }
}

//--------------------------------------
// 오브젝트 중심을 원점으로
//--------------------------------------
void MyOpenGLCore::centerObjectAtOrigin()
{
    if (m_vertices.empty()) return;

    QVector3D center(0.f, 0.f, 0.f);
    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        center += QVector3D(m_vertices[i], m_vertices[i+1], m_vertices[i+2]);
    }
    center /= (m_vertices.size() / 3.0f);

    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        m_vertices[i+0] -= center.x();
        m_vertices[i+1] -= center.y();
        m_vertices[i+2] -= center.z();
    }
    qDebug() << "Object centered at origin. Center offset was:" << center;
}

//--------------------------------------
// 활성화된 속성과 유니폼 확인
//--------------------------------------
void MyOpenGLCore::activeVertexInputAttributesAndIndices()
{
    GLint nAttribs, maxLen;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &nAttribs);
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLen);

    GLchar* name = (GLchar*) malloc(maxLen);
    qDebug() << " Index | Name";
    qDebug() << "-------------------------------------";
    for (int i = 0; i < nAttribs; ++i) {
        GLint size;
        GLenum type;
        GLsizei written;
        glGetActiveAttrib(m_program, i, maxLen, &written, &size, &type, name);
        GLint location = glGetAttribLocation(m_program, name);
        qDebug().noquote() << QString::asprintf(" %-5d | %s", location, name);
    }
    free(name);
}

void MyOpenGLCore::activeUniformVariables()
{
    GLint nUniforms, maxLen;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &nUniforms);
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);

    GLchar* name = (GLchar*) malloc(maxLen);
    qDebug() << " Location | Name";
    qDebug() << "-------------------------------------";
    for (int i = 0; i < nUniforms; ++i) {
        GLint size;
        GLenum type;
        GLsizei written;
        glGetActiveUniform(m_program, i, maxLen, &written, &size, &type, name);
        GLint location = glGetUniformLocation(m_program, name);
        qDebug().noquote() << QString::asprintf(" %-5d | %s", location, name);
    }
    free(name);
}

//--------------------------------------
// (기존에 쓰이던) 셰이더 상태 체크 함수들
//--------------------------------------
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

void MyOpenGLCore::generateTangents(const std::vector<GLfloat> &vertices,
                                    const std::vector<GLfloat> &normals,
                                    const std::vector<GLfloat> &texCoords,
                                    const std::vector<GLuint> &indices,
                                    std::vector<GLfloat> &tangents)
{
    // tangents: 정점 개수만큼 vec4가 필요.
    // (x,y,z) = 탄젠트, w = handedness(±1)
    size_t numVerts = vertices.size() / 3;
    tangents.clear();
    tangents.resize(numVerts * 4, 0.0f);  // 4개씩 (x,y,z,w)

    // 임시로 각 정점에 tangent, bitangent를 누적해서 더함
    // 마지막에 정규화(normalize)할 예정
    std::vector<QVector3D> tan1(numVerts, QVector3D(0,0,0));
    std::vector<QVector3D> bitan1(numVerts, QVector3D(0,0,0));

    // 삼각형(인덱스 3개씩) 순회
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        GLuint i0 = indices[i + 0];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        // 각 정점의 위치
        QVector3D v0(vertices[i0*3+0], vertices[i0*3+1], vertices[i0*3+2]);
        QVector3D v1(vertices[i1*3+0], vertices[i1*3+1], vertices[i1*3+2]);
        QVector3D v2(vertices[i2*3+0], vertices[i2*3+1], vertices[i2*3+2]);

        // 각 정점의 UV
        float u0 = texCoords[i0*2+0];
        float v0_ = texCoords[i0*2+1];
        float u1 = texCoords[i1*2+0];
        float v1_ = texCoords[i1*2+1];
        float u2 = texCoords[i2*2+0];
        float v2_ = texCoords[i2*2+1];

        // 가장 간단한 계산: (MikkTSpace 등의 고급 방법도 있음)
        QVector3D e1 = v1 - v0;
        QVector3D e2 = v2 - v0;
        float x1 = u1 - u0;
        float x2 = u2 - u0;
        float y1 = v1_ - v0_;
        float y2 = v2_ - v0_;
        float r = x1*y2 - x2*y1;
        float f = (fabs(r) < 1e-10f) ? 1.0f : (1.0f / r);

        // 탄젠트, 비탄젠트
        QVector3D T = f * (e1 * y2 - e2 * y1);
        QVector3D B = f * (e2 * x1 - e1 * x2);

        // 각 정점에 누적
        tan1[i0] += T; tan1[i1] += T; tan1[i2] += T;
        bitan1[i0] += B; bitan1[i1] += B; bitan1[i2] += B;
    }

    // 이제 각 정점별로 T를 N과 직교화하고, B와의 관계로 부호 w를 구함
    for (size_t i = 0; i < numVerts; i++)
    {
        QVector3D n(normals[i*3+0], normals[i*3+1], normals[i*3+2]);
        QVector3D t = tan1[i];

        // 탄젠트를 normal과 직교화 (Gram-Schmidt)
        // t' = normalize( t - ( n dot t ) * n )
        QVector3D tOrtho = (t - QVector3D::dotProduct(n, t)*n).normalized();

        // handedness = sign( (n x t) dot b ) (±1)
        // binormal = cross( n, t )이므로 여기에 B가 같은 방향이면 +1, 반대면 -1
        QVector3D b = bitan1[i];
        float handedness = (QVector3D::dotProduct(QVector3D::crossProduct(n, t), b) < 0.0f) ? -1.0f : 1.0f;

        // 결과를 tangents 벡터에 저장
        tangents[i*4 + 0] = tOrtho.x();
        tangents[i*4 + 1] = tOrtho.y();
        tangents[i*4 + 2] = tOrtho.z();
        tangents[i*4 + 3] = handedness;
    }
}
