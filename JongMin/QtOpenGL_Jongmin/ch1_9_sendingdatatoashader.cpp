#include "ch1_9_SendingDataToAShader.h"
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QFile>
#include <QTextStream>
#include <QMatrix4x4>


//1-10 Getting a list of active uniform variables 까지 포함임
ch1_9_SendingDataToAShader::~ch1_9_SendingDataToAShader()
{

}

void ch1_9_SendingDataToAShader::initializeGL()
{

    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // 화면 초기화 색상 (검은색)
    float vertices[] = {
        0.0f,  0.8f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    const GLubyte* version = glGetString(GL_VERSION);
    printf("OpenGL Version: %s\n", version);

    VAO.create();
    VAO.bind();

    VBO.create();   //glGenBuffer
    VBO.bind();     //glBindBuffer
    VBO.allocate(vertices, sizeof(vertices));   //glBufferData

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    VBO.release();



    colorVBO.create();
    colorVBO.bind();
    colorVBO.allocate(colorData, sizeof(colorData));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 , nullptr);
    glEnableVertexAttribArray(1);
    colorVBO.release();



    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    if(vertShader == 0)
    {
        qDebug() << "Shader not Created\n";
        exit(1);
    }

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    if(fragShader == 0)
    {
        qDebug() << "Shader not Created\n";
        exit(1);
    }

    const GLchar* vertshaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/basic.vert");
    const GLchar* vertcodeArray[] = {vertshaderCode};
    glShaderSource( vertShader, 1, vertcodeArray, NULL );

    const GLchar* fragshaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/fragShader.frag");
    const GLchar* fragcodeArray[] = {fragshaderCode};
    glShaderSource( fragShader, 1, fragcodeArray, NULL );

    glCompileShader(vertShader);
    glCompileShader(fragShader);

    GLint status;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(vertShader, 512, nullptr, buffer);
        qDebug() << "Vertex Shader CompileD:" << buffer;
    }

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(fragShader, 512, nullptr, buffer);
        qDebug() << "Fragment Shader CompileD:" << buffer;
    }

    programHandle = glCreateProgram();
    if( 0 == programHandle )
    {
        qDebug() << "Error Creating Program Object\n";
        exit(1);
    }


    glAttachShader(programHandle, vertShader);
    glAttachShader(programHandle, fragShader);


    glLinkProgram(programHandle);

    GLint linkStatus;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(programHandle, 512, nullptr, buffer);
        qDebug() << "Shader Program LinkD:" << buffer;
    }

    GLint nUniforms, maxLen;
    glGetProgramiv( programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH,
                   &maxLen);
    glGetProgramiv( programHandle, GL_ACTIVE_UNIFORMS,
                   &nUniforms);

    GLchar * name = (GLchar *) malloc( maxLen );
    GLint size, location;
    GLsizei written;
    GLenum type;
    printf(" Location | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( programHandle, i, maxLen, &written,
                           &size, &type, name );
        location = glGetUniformLocation(programHandle, name);
        printf(" %-8d | %s\n", location, name);
        qDebug() << "location : " <<location;
    }
    free(name);


    glUseProgram(programHandle);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ch1_9_SendingDataToAShader::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programHandle); //진짜 이거때문에 정신 나갈 뻔 했는데 Matrix업데이트 전에 glUseProgram 해야 함. 진짜 컴퓨터 죽일 뻔 했네 그냥 아휴

    QMatrix4x4 rotationMatrix;
    float angle = 10.0f;
    rotationMatrix.setToIdentity(); // 단위 행렬로 초기화
    qDebug() << rotationMatrix;
    rotationMatrix.rotate(angle, QVector3D(0.0f,0.0f,1.0f));
    qDebug() << rotationMatrix;
    GLuint location = glGetUniformLocation(programHandle,"RotationMatrix");
    if( location >= 0 )
    {
        qDebug() <<location;
        glUniformMatrix4fv(location, 1, GL_FALSE, rotationMatrix.constData());
    }

    VAO.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
    VAO.release();
}

const char* ch1_9_SendingDataToAShader::loadShaderAsString(const QString &filePath)
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
