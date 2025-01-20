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
    m_uniformModelMatrix(-1),
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

    initializeFBO();
    // 7. “fbo 텍스처를 표시할” 큐브 지오메트리 생성
    createCubeGeometryWithNormals();

    // 6. 기본 OpenGL 상태 설정
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // 6장 이미지 경로
    std::vector<QString> cubeFaces = {
        "../../sky/right.jpg",  // +x
        "../../sky/left.jpg",   // -x
        "../../sky/top.jpg",    // +y
        "../../sky/bottom.jpg", // -y
        "../../sky/front.jpg",  // +z
        "../../sky/back.jpg"    // -z
    };

    loadCubeMap(cubeFaces);
    createSkyboxGeometry();
    loadSkyboxShaders("../../skybox.vert", "../../skybox.frag");

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

void MyOpenGLCore::initializeFBO()
{
    // 1. FBO 생성 및 바인딩
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // 2. 텍스처 생성 및 설정
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // 텍스처 필터링 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 텍스처를 FBO에 부착
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    // 3. 깊이 버퍼 생성 및 부착
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // 4. FBO 완성 확인
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        qWarning() << "FBO initialization failed!";
    } else {
        qDebug() << "FBO successfully initialized.";
    }

    // 바인딩 해제
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
// void MyOpenGLCore::render()
// {
//     // 1. FBO로 렌더링 (텍스처 생성)
//     glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);  // FBO 바인딩
//     glViewport(0, 0, 512, 512);                // FBO 텍스처 크기에 맞춤
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // 기존 장면 렌더링 로직 (FBO에 씬 렌더링)
//     glUseProgram(m_program);
//     setMatrices();

//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, m_texture);
//     glUniform1i(glGetUniformLocation(m_program, "Tex1"), 0);  // 기존 텍스처 유지

//     glBindVertexArray(m_vao);
//     glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
//     glBindVertexArray(0);

//     // FBO 바인딩 해제 및 기본 프레임버퍼 복원
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // 2. 화면에 최종 결과 렌더링
//     glViewport(0, 0, 800, 600);  // 화면 전체 크기 복원
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // FBO에 렌더링된 텍스처를 적용하여 최종 씬 렌더링
//     glUseProgram(m_program);
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, m_fboTexture);

//     GLint texLoc = glGetUniformLocation(m_program, "Texture");
//     if (texLoc >= 0) {
//         glUniform1i(texLoc, 0);  // FBO에서 생성된 텍스처 사용
//     } else {
//         qDebug() << "Texture uniform location not found!";
//     }

//     glBindVertexArray(m_vao);
//     glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
//     glBindVertexArray(0);
// }

void MyOpenGLCore::render()
{
    //----------------------------------
    // [1] fbo로 렌더링 (씬 캡처)
    //----------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // fbo 텍스처 해상도에 맞게 뷰포트 세팅(512x512)
    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // (1-1) 뷰 행렬, 투영 행렬 등 설정
    // 카메라 transform 업데이트
    m_viewMatrix.setToIdentity();
    updateCameraView(m_viewMatrix);
    m_viewMatrix.translate(0.0f, 0.0f, -24.0f);

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 500.0f);

    // (1-2) 셰이더 사용
    glUseProgram(m_program);

    // (1-3) 기존 오브젝트(예: 모델) 그리기
    setMatrices(); // 내부에서 모델행렬 합성, 조명 등 Uniform 처리
    (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // glUniform1i(glGetUniformLocation(m_program, "Tex1"), 0); // 필요 시

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // fbo 해제
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------
    // [2] 기본 프레임버퍼로 렌더링
    //----------------------------------
    glViewport(0, 0, 800, 600); // 실제 윈도우 크기
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 카메라(또는 동일하게) 다시 셋업
    m_viewMatrix.setToIdentity();
    updateCameraView(m_viewMatrix);
    m_viewMatrix.translate(0.0f, 0.0f, -3.0f);

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 500.0f);

    // [2-2] fbo 결과 텍스처를 큐브에 매핑
    glUseProgram(m_program);
    setMatrices2(); // 모델행렬(큐브 위치) 등 필요하다면 약간 다르게 해도 됨

    // 예: 큐브만 살짝 앞쪽에 두어 화면에서 보이도록
    // 실제로는 setMatrices()를 조금 수정하거나
    // QMatrix4x4 model;
    // model.translate(5.f, 0.f, 0.f);
    // ... 등을 이용해 위치 조정 가능

    // (fbo 텍스처 바인딩)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    GLint loc = glGetUniformLocation(m_program, "Tex1");
    if (loc >= 0) {
        glUniform1i(loc, 0);
    }

    glBindVertexArray(m_fboCubeVAO);
    glDrawElements(GL_TRIANGLES, m_cubeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// void MyOpenGLCore::render()
// {
//     // 1. 컬러/깊이 버퍼 초기화
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // 2. 카메라 변환행렬: m_viewMatrix, m_projectionMatrix 통일
//     m_viewMatrix.setToIdentity();
//     updateCameraView(m_viewMatrix);
//     // 예시로, 카메라 전체를 z축 -24만큼 뒤로 빼고,
//     // updateCameraView로 m_cameraPosition, m_cameraRotation 적용
//     m_viewMatrix.translate(0.0f, 0.0f, -24.0f);

//     m_projectionMatrix.setToIdentity();
//     // 스카이박스 크기가 100배라 far=100으로 부족하므로, 넉넉히 500
//     m_projectionMatrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 500.0f);

//     //---------------------------------
//     // 4. 일반 오브젝트(노멀 매핑) 렌더
//     //---------------------------------

//     // 모델 변환 행렬 (예: 회전, 이동 포함)
//     QMatrix4x4 modelMatrix;
//     modelMatrix.setToIdentity();

//     glUseProgram(m_program);

//     // 7. 텍스처 설정
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, m_texture);
//     GLint texLoc = glGetUniformLocation(m_program, "Tex1");
//     if (texLoc < 0) {
//         qDebug() << "Tex1 uniform location not found!";
//     }

//     // 5. setMatrices()에서 m_viewMatrix, m_projectionMatrix를 참조해
//     //    모델 행렬 합성, 조명 좌표 변환, 유니폼 설정
//     setMatrices();

//     // One pixel white texture
//     GLuint whiteTexHandle;
//     GLubyte whiteTex[] = { 255, 255, 255, 255 };
//     glActiveTexture(GL_TEXTURE1);
//     glGenTextures(1, &whiteTexHandle);
//     glBindTexture(GL_TEXTURE_2D,whiteTexHandle);
//     glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,
//                  GL_UNSIGNED_BYTE,whiteTex);

//     GLenum err;
//     while ((err = glGetError()) != GL_NO_ERROR) {
//         qDebug() << "OpenGL error after setting uniforms:" << err;
//     }

//     // 6. 실제 지오메트리 렌더링
//     glBindVertexArray(m_vao);
//     glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
//     glBindVertexArray(0);

//     // // Bind to texture's FBO
//     // glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//     // glViewport(0,0,512,512); // Viewport for the texture
//     // // Use the "white" texture here
//     // int loc = glGetUniformLocation(m_program, "Texture");
//     // glUniform1i(loc, 1);
//     // // Unbind texture's FBO (back to default FB)
//     // glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     // glViewport(0,0,800,600); // Viewport for main window
//     // // Use the texture that is associated with the FBO
//     // loc = glGetUniformLocation(m_program, "Texture");
//     // glUniform1i(loc, 0);

//     // // 6. 실제 지오메트리 렌더링
//     // glBindVertexArray(m_vao);
//     // glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
//     // glBindVertexArray(0);

// }


// void MyOpenGLCore::render()
// {
//     // 1. 컬러/깊이 버퍼 초기화
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // 2. 카메라 변환행렬: m_viewMatrix, m_projectionMatrix 통일
//     m_viewMatrix.setToIdentity();
//     updateCameraView(m_viewMatrix);
//     // 예시로, 카메라 전체를 z축 -24만큼 뒤로 빼고,
//     // updateCameraView로 m_cameraPosition, m_cameraRotation 적용
//     m_viewMatrix.translate(0.0f, 0.0f, -24.0f);

//     m_projectionMatrix.setToIdentity();
//     // 스카이박스 크기가 100배라 far=100으로 부족하므로, 넉넉히 500
//     m_projectionMatrix.perspective(45.0f, 4.0f/3.0f, 0.1f, 500.0f);

//     //---------------------------------
//     // 3. 스카이박스 먼저 렌더링
//     // ---------------------------------
//     renderSkybox(m_viewMatrix, m_projectionMatrix);

//     //---------------------------------
//     // 4. 일반 오브젝트(노멀 매핑) 렌더
//     //---------------------------------

//     // 모델 변환 행렬 (예: 회전, 이동 포함)
//     QMatrix4x4 modelMatrix;
//     modelMatrix.setToIdentity();
//     // 모델의 위치를 설정하거나 조정이 필요한 경우 추가
//     // modelMatrix.translate(0.0f, 0.0f, 0.0f);
//     // modelMatrix.rotate(45, 0, 1, 0);  // 예제 회전

//     glUseProgram(m_program);
//     // GLint drawSkybox = 0;
//     // glUniform1i(glGetUniformLocation(m_program, "DrawSkyBox"), drawSkybox);

//     // 6. 추가 유니폼 설정 (카메라 위치, 머티리얼 컬러, 반사 계수)
//     QVector3D cameraPos = m_cameraPosition;
//     // QVector3D cameraPos = m_cameraPosition;
//     glUniform3f(glGetUniformLocation(m_program, "WorldCameraPosition"),
//                 cameraPos.x(), cameraPos.y(), cameraPos.z());

//     // 7. 텍스처 설정
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, m_texture);
//     GLint texLoc = glGetUniformLocation(m_program, "Tex1");
//     if (texLoc < 0) {
//         qDebug() << "Tex1 uniform location not found!";
//     }

//     // 5. setMatrices()에서 m_viewMatrix, m_projectionMatrix를 참조해
//     //    모델 행렬 합성, 조명 좌표 변환, 유니폼 설정
//     setMatrices();

//     setFBO();

//     GLenum err;
//     while ((err = glGetError()) != GL_NO_ERROR) {
//         qDebug() << "OpenGL error after setting uniforms:" << err;
//     }

//     // 6. 실제 지오메트리 렌더링
//     glBindVertexArray(m_vao);
//     glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
//     glBindVertexArray(0);
// }
//--------------------------------------
// 조명, 행렬 관련 설정(뷰, 투영 등)
//--------------------------------------
void MyOpenGLCore::setMatrices()
{
    // 1) 모델 행렬(개별 오브젝트 변환)
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    // 필요 시 오브젝트 자체 회전 or 이동
    // modelMatrix.rotate(...);
    // modelMatrix.translate(...);

    // 2) 모델뷰
    QMatrix4x4 modelViewMatrix = m_viewMatrix * modelMatrix;

    // 3) 노멀 매트릭스
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // 4) MVP
    QMatrix4x4 mvpMatrix = m_projectionMatrix * modelViewMatrix;

    // 5) 조명 좌표(월드->뷰)
    QVector4D lightPosWorld(300.f, 300.f, 300.f, 1.f);
    QVector4D lightPosView = m_viewMatrix * lightPosWorld;
    GLfloat lightPosition[4] = {
        lightPosView.x(), lightPosView.y(),
        lightPosView.z(), lightPosView.w()
    };
    glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, lightPosition);

    // 머티리얼/조명 세팅 (기존 동일)
    GLfloat lightIntensity[3] = { 1.2f, 0.8f, 0.3f };
    glUniform3fv(glGetUniformLocation(m_program, "LightIntensity"), 1, lightIntensity);

    GLfloat ka[3] = { 0.3f, 0.2f, 0.0f };
    GLfloat kd[3] = { 1.0f, 0.6f, 0.2f };
    GLfloat ks[3] = { 1.0f, 0.7f, 0.3f };
    GLfloat shininess = 32.0f;
    glUniform3fv(glGetUniformLocation(m_program, "Ka"), 1, ka);
    glUniform3fv(glGetUniformLocation(m_program, "Kd"), 1, kd);
    glUniform3fv(glGetUniformLocation(m_program, "Ks"), 1, ks);
    glUniform1f(glGetUniformLocation(m_program, "Shininess"), shininess);

    // Uniform 변수 로드
    m_uniformModelMatrix    = glGetUniformLocation(m_program, "ModelMatrix");
    m_uniformModelViewMatrix    = glGetUniformLocation(m_program, "ModelViewMatrix");
    m_uniformNormalMatrix       = glGetUniformLocation(m_program, "NormalMatrix");
    m_uniformProjectionMatrix   = glGetUniformLocation(m_program, "ProjectionMatrix");
    m_uniformMVP                = glGetUniformLocation(m_program, "MVP");

    // 6) 행렬 유니폼 전달
    glUniformMatrix4fv(m_uniformModelMatrix,   1, GL_FALSE, modelMatrix.constData());
    glUniformMatrix4fv(m_uniformModelViewMatrix,   1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix,      1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix,  1, GL_FALSE, m_projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP,               1, GL_FALSE, mvpMatrix.constData());
}
void MyOpenGLCore::setMatrices2()
{
    // 1) 모델 행렬(개별 오브젝트 변환)
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    // 필요 시 오브젝트 자체 회전 or 이동
    // modelMatrix.rotate(...);
    // modelMatrix.translate(...);

    // 2) 모델뷰
    QMatrix4x4 modelViewMatrix = m_viewMatrix * modelMatrix;

    // 3) 노멀 매트릭스
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    // 4) MVP
    QMatrix4x4 mvpMatrix = m_projectionMatrix * modelViewMatrix;

    // 5) 조명 좌표(월드->뷰)
    QVector4D lightPosWorld(300.f, 300.f, 300.f, 1.f);
    QVector4D lightPosView = m_viewMatrix * lightPosWorld;
    GLfloat lightPosition[4] = {
        lightPosView.x(), lightPosView.y(),
        lightPosView.z(), lightPosView.w()
    };
    glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, lightPosition);

    // 머티리얼/조명 세팅 (기존 동일)
    GLfloat lightIntensity[3] = { 1.2f, 0.8f, 0.3f };
    glUniform3fv(glGetUniformLocation(m_program, "LightIntensity"), 1, lightIntensity);

    GLfloat ka[3] = { 0.3f, 0.2f, 0.0f };
    GLfloat kd[3] = { 1.0f, 0.6f, 0.2f };
    GLfloat ks[3] = { 1.0f, 0.7f, 0.3f };
    GLfloat shininess = 32.0f;
    glUniform3fv(glGetUniformLocation(m_program, "Ka"), 1, ka);
    glUniform3fv(glGetUniformLocation(m_program, "Kd"), 1, kd);
    glUniform3fv(glGetUniformLocation(m_program, "Ks"), 1, ks);
    glUniform1f(glGetUniformLocation(m_program, "Shininess"), shininess);

    // Uniform 변수 로드
    m_uniformModelMatrix    = glGetUniformLocation(m_program, "ModelMatrix");
    m_uniformModelViewMatrix    = glGetUniformLocation(m_program, "ModelViewMatrix");
    m_uniformNormalMatrix       = glGetUniformLocation(m_program, "NormalMatrix");
    m_uniformProjectionMatrix   = glGetUniformLocation(m_program, "ProjectionMatrix");
    m_uniformMVP                = glGetUniformLocation(m_program, "MVP");

    // 6) 행렬 유니폼 전달
    glUniformMatrix4fv(m_uniformModelMatrix,   1, GL_FALSE, modelMatrix.constData());
    glUniformMatrix4fv(m_uniformModelViewMatrix,   1, GL_FALSE, modelViewMatrix.constData());
    glUniformMatrix3fv(m_uniformNormalMatrix,      1, GL_FALSE, normalMatrix.constData());
    glUniformMatrix4fv(m_uniformProjectionMatrix,  1, GL_FALSE, m_projectionMatrix.constData());
    glUniformMatrix4fv(m_uniformMVP,               1, GL_FALSE, mvpMatrix.constData());
}
//--------------------------------------
// FBO 설정
//--------------------------------------
void MyOpenGLCore::setFBO(){
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // Create the texture object
    GLuint renderTex;
    glGenTextures(1, &renderTex);
    glActiveTexture(GL_TEXTURE0); // Use texture unit 0
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,
                 GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, renderTex, 0);
    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          512, 512);
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthBuf);
    // Set the target for the fragment shader outputs
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // // One pixel white texture
    // GLuint whiteTexHandle;
    // GLubyte whiteTex[] = { 255, 255, 255, 255 };
    // glActiveTexture(GL_TEXTURE1);
    // glGenTextures(1, &whiteTexHandle);
    // glBindTexture(GL_TEXTURE_2D,whiteTexHandle);
    // glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,
    //              GL_UNSIGNED_BYTE,whiteTex);

    // // Bind to texture's FBO
    // glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // glViewport(0,0,512,512); // Viewport for the texture
    // // Use the "white" texture here
    // int loc = glGetUniformLocation(m_program, "Texture");
    // glUniform1i(loc, 1);
    // // Unbind texture's FBO (back to default FB)
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glViewport(0,0,800,600); // Viewport for main window
    // // Use the texture that is associated with the FBO
    // loc = glGetUniformLocation(m_program, "Texture");
    // glUniform1i(loc, 0);
}

//--------------------------------------
// 카메라 변환 적용
//--------------------------------------
void MyOpenGLCore::updateCameraView(QMatrix4x4 &viewMatrix)
{
    // 회전은 쿼터니언의 역(컨주게이트) 곱 적용
    viewMatrix.rotate(m_cameraRotation.conjugated());
    viewMatrix.translate(-m_cameraPosition);
    // viewMatrix.translate(-m_cameraPosition);
    // viewMatrix.rotate(m_cameraRotation.conjugated());
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

    // 마우스 x 이동 -> yaw, y 이동 -> pitch
    float yawDelta   = -delta.x() * m_rotationSpeed;
    float pitchDelta = -delta.y() * m_rotationSpeed;

    // 누적
    m_yaw   += yawDelta;
    m_pitch += pitchDelta;

    // Pitch 각도 제한 ( -89~ +89도 )
    if(m_pitch >  89.0f) m_pitch =  89.0f;
    if(m_pitch < -89.0f) m_pitch = -89.0f;

    // 쿼터니언으로 재계산
    QQuaternion yawQ   = QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, m_yaw);
    QQuaternion pitchQ = QQuaternion::fromAxisAndAngle(1.f, 0.f, 0.f, m_pitch);

    // 최종 카메라 회전
    // 전역 Y축으로 yaw, yaw가 적용된 좌표계에서 로컬 X축으로 pitch
    m_cameraRotation = yawQ * pitchQ;

    // qDebug() << "yawDelta =" << yawDelta << "pitchDelta =" << pitchDelta;
    // qDebug() << "m_yaw =" << m_yaw << "m_pitch =" << m_pitch;

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

void MyOpenGLCore::loadCubeMap(const std::vector<QString> &faces)
{
    // faces.size()는 보통 6이 되어야 함 (+x, -x, +y, -y, +z, -z)
    if (faces.size() != 6) {
        qWarning() << "CubeMap requires 6 texture paths.";
        return;
    }
    glGenTextures(1, &m_cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTexture);

    // +x, -x, +y, -y, +z, -z 순서로 로드
    for (int i = 0; i < 6; i++) {
        QImage img;
        if (!img.load(faces[i])) {
            qWarning() << "Failed to load cubemap face:" << faces[i];
            continue;
        }
        qDebug() << "Cubemap face" << i << faces[i] << "loaded with size"
                 << img.width() << "x" << img.height();
        QImage glImg = img.convertToFormat(QImage::Format_RGBA8888);

        // OpenGL로 복사
        // GL_TEXTURE_CUBE_MAP_POSITIVE_X + i 로 지정
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGBA,
                     glImg.width(),
                     glImg.height(),
                     0, GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     glImg.bits());
    }

    // 큐브맵 필터/래핑 설정
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // 보통 큐브맵은 clamp 해도 되지만, 원하는 경우 GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    qDebug() << "CubeMap texture loaded.";
}

void MyOpenGLCore::createSkyboxGeometry()
{
    float skyboxVertices[] = {
        // 뒤 z = -1
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // 앞 z = +1
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // 아래
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        // 위
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,

        // 좌
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,

        // 우
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f
    };

    for (int i=0; i < (int) sizeof(skyboxVertices); i++){
        skyboxVertices[i] *= 100;
    }

    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);
    glBindVertexArray(m_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // layout(location = 0) in vec3 aPos; (예: skybox.vert에서)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    qDebug() << "Skybox geometry created.";
}

void MyOpenGLCore::loadSkyboxShaders(const QString &vertPath, const QString &fragPath)
{
    // 간단히 vertex/fragment 로딩, 컴파일, 링크 (기존 loadShaders와 유사)
    // 여기선 새 m_skyboxProgram 에 저장
    auto readFile = [&](const QString &path) -> QByteArray {
        QFile f(path);
        if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){
            qWarning() << "Skybox shader file not found:" << path;
            return QByteArray();
        }
        return f.readAll();
    };

    QByteArray vsSrc = readFile(vertPath);
    QByteArray fsSrc = readFile(fragPath);
    if(vsSrc.isEmpty()|| fsSrc.isEmpty()){
        qWarning() << "Skybox shader source empty.";
        return;
    }

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vsC = vsSrc.constData();
    glShaderSource(vs, 1, &vsC, nullptr);
    glCompileShader(vs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(!success){
        char log[512];
        glGetShaderInfoLog(vs,512,nullptr,log);
        qWarning() << "Skybox vertex shader error:\n" << log;
        glDeleteShader(vs);
        return;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsC = fsSrc.constData();
    glShaderSource(fs,1,&fsC,nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success){
        char log[512];
        glGetShaderInfoLog(fs,512,nullptr,log);
        qWarning() << "Skybox fragment shader error:\n" << log;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return;
    }

    m_skyboxProgram = glCreateProgram();
    glAttachShader(m_skyboxProgram, vs);
    glAttachShader(m_skyboxProgram, fs);
    glLinkProgram(m_skyboxProgram);

    glGetProgramiv(m_skyboxProgram, GL_LINK_STATUS, &success);
    if(!success){
        char log[512];
        glGetProgramInfoLog(m_skyboxProgram,512,nullptr,log);
        qWarning() << "Skybox program link error:\n" << log;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return;
    }
    // 삭제
    glDeleteShader(vs);
    glDeleteShader(fs);

    qDebug() << "Skybox shaders loaded. Program =" << m_skyboxProgram;
}

void MyOpenGLCore::renderSkybox(const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix)
{
    glDepthFunc(GL_LEQUAL);  // 스카이박스는 배경이므로
    glDisable(GL_CULL_FACE);
    // 기존에 glCullFace(GL_BACK)가 설정되어 있다면, 스카이박스에서는
    // 그걸 잠시 GL_FRONT로 바꿔서 "안쪽 면"을 그리도록 함
    // glCullFace(GL_FRONT);

    glUseProgram(m_skyboxProgram);

    // viewMatrix에서 위치 이동 제거(회전만 남김)
    QMatrix4x4 rotOnly = viewMatrix;
    rotOnly.setColumn(3, QVector4D(0,0,0,1)); // 위치 부분(행렬 4열)을 (0,0,0,1)로
    // qDebug() << rotOnly;

    // 셰이더 uniform 세팅
    GLint locView = glGetUniformLocation(m_skyboxProgram, "uView");
    GLint locProj = glGetUniformLocation(m_skyboxProgram, "uProjection");
    if(locView>=0) glUniformMatrix4fv(locView,1,GL_FALSE,rotOnly.constData());
    else{
        qDebug() << "locView";
    }
    if(locProj>=0) glUniformMatrix4fv(locProj,1,GL_FALSE,projectionMatrix.constData());
    else{
        qDebug() << "locProj";
    }
    // 큐브맵 바인딩, samplerCube는 texture unit 0 가정
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTexture);
    GLint skyLoc = glGetUniformLocation(m_skyboxProgram, "skybox");
    if(skyLoc>=0) glUniform1i(skyLoc,0);
    else{
        qDebug() << "skyLoc";
    }

    // 스카이박스 VAO 렌더
    glBindVertexArray(m_skyboxVAO);
    // 정점 36개(6면*6)
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // 원복
    glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
}

void MyOpenGLCore::createCubeGeometry()
{
    // 간단한 1x1x1 큐브 정점 데이터 예시
    // 텍스처 좌표 (u,v)까지 넣어주자
    // (정점당 x,y,z + u,v) => 위치 3 + uv 2 = 5 floats
    // 실제론 취향껏 배치 가능
    float cubeData[] = {
        //   x     y     z      u     v
        // 앞면
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

        // 뒷면
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,

        // 좌측면
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,

        // 우측면
        0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,

        // 윗면
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,

        // 아랫면
        0.5f, -0.5f,  0.5f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
    };

    // 인덱스(각 면을 2개의 삼각형으로)
    unsigned int cubeIndices[] = {
        // 앞면
        0,1,2,   2,3,0,
        // 뒷면
        4,5,6,   6,7,4,
        // 좌측면
        8,9,10,  10,11,8,
        // 우측면
        12,13,14,14,15,12,
        // 윗면
        16,17,18,18,19,16,
        // 아랫면
        20,21,22,22,23,20
    };
    m_cubeIndexCount = sizeof(cubeIndices)/sizeof(cubeIndices[0]);

    glGenVertexArrays(1, &m_fboCubeVAO);
    glBindVertexArray(m_fboCubeVAO);

    glGenBuffers(1, &m_fboCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_fboCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);

    glGenBuffers(1, &m_fboCubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fboCubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices),
                 cubeIndices, GL_STATIC_DRAW);

    // 레이아웃(location=0) = 위치(x,y,z)
    // 레이아웃(location=1) = 텍스처 좌표(u,v)
    // 1개의 정점에 float 5개: (x,y,z,u,v)
    // -> stride = 5 * sizeof(float)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)(3*sizeof(float)));

    // Unbind
    glBindVertexArray(0);

    qDebug() << "[MyOpenGLCore::createCubeGeometry] Done.";
}

void MyOpenGLCore::createCubeGeometryWithNormals()
{
    // 각 면마다 4개의 정점
    // 정점 하나에 (x,y,z) + (nx,ny,nz) + (u,v) = 8 floats
    // 예: 앞면 normal은 (0,0,+1)
    float cubeData[] = {
        // -- 앞면 (z=+0.5), normal = (0,0,+1)
        //   x     y     z      nx    ny    nz     u     v
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, +1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 0.0f, +1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 0.0f, +1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, +1.0f, 0.0f, 1.0f,

        // -- 뒷면 (z=-0.5), normal = (0,0,-1)
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

        // -- 좌측면 (x=-0.5), normal = (-1,0,0)
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

        // -- 우측면 (x=+0.5), normal = (+1,0,0)
        0.5f,  0.5f,  0.5f,  +1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  +1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  +1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  +1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

        // -- 윗면 (y=+0.5), normal = (0,+1,0)
        0.5f,  0.5f,  0.5f,   0.0f,+1.0f,0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,+1.0f,0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,+1.0f,0.0f,   0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   0.0f,+1.0f,0.0f,   1.0f, 1.0f,

        // -- 아랫면 (y=-0.5), normal = (0,-1,0)
        0.5f, -0.5f,  0.5f,   0.0f,-1.0f,0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,-1.0f,0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,-1.0f,0.0f,   1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f,-1.0f,0.0f,   0.0f, 0.0f,
    };

    // 인덱스(각 면을 2개의 삼각형으로)
    unsigned int cubeIndices[] = {
        // 앞면
        0,1,2,   2,3,0,
        // 뒷면
        4,5,6,   6,7,4,
        // 좌측면
        8,9,10,  10,11,8,
        // 우측면
        12,13,14,14,15,12,
        // 윗면
        16,17,18,18,19,16,
        // 아랫면
        20,21,22,22,23,20
    };
    m_cubeIndexCount = sizeof(cubeIndices)/sizeof(cubeIndices[0]);

    // VAO, VBO, EBO 생성
    glGenVertexArrays(1, &m_fboCubeVAO);
    glBindVertexArray(m_fboCubeVAO);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices),
                 cubeIndices, GL_STATIC_DRAW);

    // ------------ 주의: attribute layout 설정 ------------
    //  정점 1개 당 8개의 float:
    //    [0..2]: (x,y,z)
    //    [3..5]: (nx,ny,nz)
    //    [6..7]: (u,v)
    // stride = 8 * sizeof(float)
    // position: layout(location = 0)
    // normal:   layout(location = 1)
    // texcoord: layout(location = 2)

    GLsizei stride = 8 * sizeof(float);

    // 위치
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          stride, (void*)0);

    // 노말
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          stride, (void*)(3 * sizeof(float)));

    // 텍스처 좌표
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    qDebug() << "[MyOpenGLCore::createCubeGeometryWithNormals] Done.";
}
