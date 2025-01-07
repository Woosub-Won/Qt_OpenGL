#include "ch1_7_sendingdatatoshader.h"
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QFile>
#include <QTextStream>



Ch1_7_SendingDataToShader::Ch1_7_SendingDataToShader(QWidget *parent):QOpenGLWidget(parent) {

}

Ch1_7_SendingDataToShader::~Ch1_7_SendingDataToShader()
{

}

void Ch1_7_SendingDataToShader::initializeGL()
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


    //p29. 1.Create Shader Object as follows.
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

    //2. Copy the source code (perhaps from multiple locations) into the shader object.
    const GLchar* vertshaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/basic.vert");
    const GLchar* vertcodeArray[] = {vertshaderCode};
    glShaderSource( vertShader, 1, vertcodeArray, NULL );

    const GLchar* fragshaderCode = loadShaderAsString("C:/Users/user/Desktop/QtStudy/QtOpenGL_Jongmin/Shaders/fragShader.frag");
    const GLchar* fragcodeArray[] = {fragshaderCode};
    glShaderSource( fragShader, 1, fragcodeArray, NULL );

    //3. Compile the shader
    glCompileShader(vertShader);
    glCompileShader(fragShader);

    //4. verify 는 귀찮아서 패스 ㅎㅎ 지송

    //p32. 1.Create the program object
    programHandle = glCreateProgram();
    if( 0 == programHandle )
    {
        qDebug() << "Error Creating Program Object\n";
        exit(1);
    }

    //2. Attach the shaders to the program object.
    glAttachShader(programHandle, vertShader);
    glAttachShader(programHandle, fragShader);

    //3. Link the Program
    glLinkProgram(programHandle);

    //4. Verify is so annoying. sorry

    //5. If linking is successful, install the program into the OpenGL pipeline.
    glUseProgram(programHandle);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Ch1_7_SendingDataToShader::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programHandle);
    VAO.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
    VAO.release();
}

const char* Ch1_7_SendingDataToShader::loadShaderAsString(const QString &filePath)
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
