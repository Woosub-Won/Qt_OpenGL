#include "myopenglcore.h"
#include <QFile>
#include <QTextStream>
#include <iostream>

MyOpenGLCore::MyOpenGLCore(const std::vector<GLfloat> &vertices,
                           const std::vector<GLfloat> &colors)
    : m_program(0), m_vertices(vertices), m_colors(colors)
{
}

MyOpenGLCore::~MyOpenGLCore()
{
    glDeleteProgram(m_program); // 셰이더 프로그램 삭제
}

void MyOpenGLCore::initialize()
{
    initializeOpenGLFunctions(); // OpenGL 함수 초기화

    // VAO 생성 및 바인딩
    m_vao.create();
    m_vao.bind();

    // VBO 생성 및 데이터 전송
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(m_vertices.data(), m_vertices.size() * sizeof(GLfloat));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // CBO 생성 및 데이터 전송
    m_cbo.create();
    m_cbo.bind();
    m_cbo.allocate(m_colors.data(), m_colors.size() * sizeof(GLfloat));

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    m_vao.release();
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

void MyOpenGLCore::render()
{
    glClear(GL_COLOR_BUFFER_BIT); // 화면 초기화

    glUseProgram(m_program);      // 셰이더 프로그램 활성화

    m_vao.bind();                 // VAO 바인딩
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3); // 삼각형 렌더링
    m_vao.release();              // VAO 해제
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

void MyOpenGLCore::activeVertexInputAttributesAndIndices(){
    GLint maxLength, nAttribs;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES,
                   &nAttribs);
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                   &maxLength);
    GLchar * name = (GLchar *) malloc( maxLength );

    GLint written, size, location;
    GLenum type;
    qDebug() << (" Index | Name");
    qDebug() << ("------------------------------------------------");
    for( int i = 0; i < nAttribs; i++ ) {
        glGetActiveAttrib( m_program, i, maxLength, &written,
                          &size, &type, name );
        location = glGetAttribLocation(m_program, name);
        qDebug().noquote() << QString::asprintf(" %-5d | %s",location, name);
    }
    free(name);
}
