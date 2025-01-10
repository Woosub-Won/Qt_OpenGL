#include "myopenglcore.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QMatrix4x4>
#include <QtMath>

// 생성자
MyOpenGLCore::MyOpenGLCore(const std::vector<GLfloat> &vertices,
                           const std::vector<GLfloat> &colors,
                           const std::vector<GLfloat> &texCoords)
    : m_vertices(vertices),
    m_colors(colors.empty() ? defaultColors : colors),
    m_texCoords(texCoords.empty() ? defaultTexCoords : texCoords) {

    // 기본값 여부 확인
    if (m_colors == defaultColors) {
        std::cout << "[INFO] Colors are using default values (Black/White Pattern)." << std::endl;
    } else {
        std::cout << "[INFO] Colors are custom-defined." << std::endl;
    }

    if (m_texCoords == defaultTexCoords) {
        std::cout << "[INFO] Texture coordinates are using default values (Square Corners)." << std::endl;
    } else {
        std::cout << "[INFO] Texture coordinates are custom-defined." << std::endl;
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

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO 생성 및 데이터 전송
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // CBO 생성 및 데이터 전송
    glGenBuffers(1, &m_cbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cbo);
    glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof(GLfloat), m_colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // TBO 생성 및 데이터 전송 (Texture Coordinates)
    glGenBuffers(1, &m_tbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
    glBufferData(GL_ARRAY_BUFFER, m_texCoords.size() * sizeof(GLfloat), m_texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

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

    // 셰이더 객체 삭제
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

// void MyOpenGLCore::render() {
//     glClear(GL_COLOR_BUFFER_BIT);

//     QMatrix4x4 rotationMatrix;
//     rotationMatrix.setToIdentity();
//     GLfloat angle = 10.0f;
//     rotationMatrix.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));

//     glUseProgram(m_program); // 프로그램 활성화
//     GLuint location = glGetUniformLocation(m_program, "RotationMatrix");
//     if (location >= 0) {
//         glUniformMatrix4fv(location, 1, GL_FALSE, rotationMatrix.constData());
//     } else {
//         qDebug() << "ERROR: Uniform 'RotationMatrix' not found in shader.";
//     }

//     glBindVertexArray(m_vao);
//     glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
//     glBindVertexArray(0);
// }


void MyOpenGLCore::render()
{
    glClear(GL_COLOR_BUFFER_BIT); // 화면 초기화

    glUseProgram(m_program);      // 셰이더 프로그램 활성화

    // uniform
    // QMatrix4x4 rotationMatrix;
    // rotationMatrix.setToIdentity();
    // GLfloat angle = 30;
    // rotationMatrix.rotate(angle, QVector3D(0, 0, 1));

    // GLint location = glGetUniformLocation(m_program, "RotationMatrix");
    // // -1 이 에러 반환이라 책 오류인듯?
    // if (location >= 0) {
    //     glUniformMatrix4fv(location, 1, GL_FALSE, rotationMatrix.constData());
    // }
    // else{
    //     qDebug() << "ERROR";
    // }

    // uniform block
    GLuint blockIndex = glGetUniformBlockIndex(m_program, "BlobSettings");
    GLint blockSize;
    glGetActiveUniformBlockiv(m_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    GLubyte * blockBuffer= (GLubyte *) malloc(blockSize);
    // Query for the offsets of each block variable
    const GLchar *names[] = { "InnerColor", "OuterColor",
                             "RadiusInner", "RadiusOuter" };
    GLuint indices[4];
    glGetUniformIndices(m_program, 4, names, indices);
    GLint offset[4];
    glGetActiveUniformsiv(m_program, 4, indices, GL_UNIFORM_OFFSET, offset);

    GLfloat outerColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    GLfloat innerColor[] = {1.0f, 1.0f, 0.75f, 1.0f};
    GLfloat innerRadius = 0.10f, outerRadius = 0.25f;
    memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
    memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));
    GLuint uboHandle;
    glGenBuffers( 1, &uboHandle );
    glBindBuffer( GL_UNIFORM_BUFFER, uboHandle );
    glBufferData( GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW );
    glBindBufferBase( GL_UNIFORM_BUFFER, blockIndex, uboHandle );

    glBindVertexArray(m_vao);     // VAO 바인딩
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3); // 삼각형 렌더링
    glBindVertexArray(0);         // VAO 해제
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
