#include "ch1_8_GettingAList.h"
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QFile>
#include <QTextStream>





ch1_8_GettingAList::~ch1_8_GettingAList()
{

}

void ch1_8_GettingAList::initializeGL()
{
    initializeOpenGLFunctions();

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


    VAO.create();
    VAO.bind();

    VBO.create();   //glGenBuffer
    VBO.bind();     //glBindBuffer
    VBO.allocate(vertices, sizeof(vertices));   //glBufferData
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    VBO.release();



    colorVBO.create();
    colorVBO.bind();
    colorVBO.allocate(colorData, sizeof(colorData));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 , nullptr);
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



    programHandle = glCreateProgram();
    if( 0 == programHandle )
    {
        qDebug() << "Error Creating Program Object\n";
        exit(1);
    }


    glAttachShader(programHandle, vertShader);
    glAttachShader(programHandle, fragShader);


    glLinkProgram(programHandle);

    //p.43 Retrieve the number of active attributes and the maximum length of their names using glGetProgramiv.

    GLint maxLength, nAttribs;
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

    // Allocate a buffer to hold each attribute name.
    GLchar * name = (GLchar *)malloc(maxLength);

    //Get and print information about each active attribute using glGetActiveAttrib and glGetAttribLocation.

    GLint written, size, location;
    GLenum type;
    //printf(" Index | Name\n"); //참고로 printf는 ui 꺼야 나옴
    //printf("------------------------------------------------\n");
    qDebug() << " Index | Name\n------------------------------------------------\n";
    for( int i = 0; i < nAttribs; i++ ) {
        glGetActiveAttrib( programHandle, i, maxLength, &written,
                          &size, &type, name );
        location = glGetAttribLocation(programHandle, name);
        //printf(" %-5d | %s\n",location, name);
        qDebug() << location << " | " << name;
    }
    free(name);



    glUseProgram(programHandle);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ch1_8_GettingAList::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programHandle);
    VAO.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
    VAO.release();
}

const char* ch1_8_GettingAList::loadShaderAsString(const QString &filePath)
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
