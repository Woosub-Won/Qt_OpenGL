#include "ch1_10_usinguniformblocks.h"
#include <QFile>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

ch1_10_UsingUniformBlocks::~ch1_10_UsingUniformBlocks() {

}

void ch1_10_UsingUniformBlocks::initializeGL()
{
    initializeOpenGLFunctions();
    float vertices[] = {
        // Position         // Texture Coordinates
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // Top-left
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // Top-right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Bottom-left

        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Bottom-left
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // Top-right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f   // Bottom-right
    };

    float positions[] = {
        -0.5f,  0.5f, 0.0f,  // Top-left
        0.5f,  0.5f, 0.0f,  // Top-right
        -0.5f, -0.5f, 0.0f,  // Bottom-left

        -0.5f, -0.5f, 0.0f,  // Bottom-left
        0.5f,  0.5f, 0.0f,  // Top-right
        0.5f, -0.5f, 0.0f   // Bottom-right
    };
    float texCoords[] = {
        0.0f, 1.0f,  // Top-left
        1.0f, 1.0f,  // Top-right
        0.0f, 0.0f,  // Bottom-left

        0.0f, 0.0f,  // Bottom-left
        1.0f, 1.0f,  // Top-right
        1.0f, 0.0f   // Bottom-right
    };

    VAO.create();
    VAO.bind();

    // unsigned int VBO;
    // glGenBuffers(1, &VBO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //셰이더에 전달 0번 location에 / 3개의 값을 / 데이터 크기 / 시작 위치
    // glEnableVertexAttribArray(0);

    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); //텍스쳐는 1번에, 2개의 값을 / 이만큼 간격을 두고 / 시작 위치
    // glEnableVertexAttribArray(1);

    pVBO.create();
    pVBO.bind();
    pVBO.allocate(positions,sizeof(positions));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    pVBO.release();

    tVBO.create();
    tVBO.bind();
    tVBO.allocate(texCoords,sizeof(texCoords));
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE,0,nullptr);
    glEnableVertexAttribArray(1);
    tVBO.release();


    VAO.bind();

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

    //4. verify

    glCompileShader(vertShader);
    GLint isCompiled = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);

        qDebug() << "Vertex Shader compilation error:" << &errorLog[0];
        glDeleteShader(vertShader);
        return;
    }
    else{
        qDebug() << "Vert Shader compile successful!";
    }

    glCompileShader(fragShader);
    isCompiled = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);

        qDebug() << "Vertex Shader compilation error:" << &errorLog[0];
        glDeleteShader(vertShader);
        return;
    }
    else{
        qDebug() << "Frag Shader compile successful!";
    }


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

    //4. Verify
    GLint isLinked = 0;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(programHandle, maxLength, &maxLength, &errorLog[0]);

        qDebug() << "Shader Program linking error:" << &errorLog[0];
        glDeleteProgram(programHandle);
        return;
    }
    else{
        qDebug() << "Shader Program linking successful!";
    }

    //p52
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    GLuint blockIndex = gl->glGetUniformBlockIndex(programHandle, "BlobSettings"); //유니폼 블록 인덱스 가져오기

    //ch10 _ 2
    GLint blockSize;
    gl->glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize); //해당 블록이 사용하는 메모리의 크기를 알아야 저장할 수 있음.
    GLubyte *blockBuffer = (GLubyte *)malloc(blockSize); //데이터 임시저장 버퍼 (blockBuffer)


    //ch10_3
    const GLchar *names[] = {"InnerColor", "OuterColor", "RadiusInner", "RadiusOuter"};
    GLuint indices[4];
    gl->glGetUniformIndices(programHandle, 4, names, indices);  //Uniform Block안의 변수 인덱스 반환해서 indices에 저장

    GLint offset[4];
    gl->glGetActiveUniformsiv(programHandle, 4, indices, GL_UNIFORM_OFFSET, offset); //offset도 반환

    //4ch10_4
    GLfloat outerColor[] = {0.0f, 0.0f, 0.0f, 0.0f};  // 외부 색상
    GLfloat innerColor[] = {1.0f, 1.0f, 0.75f, 1.0f}; // 내부 색상
    GLfloat innerRadius = 0.25f;                      // 내부 반지름
    GLfloat outerRadius = 0.45f;                      // 외부 반지름

    //싹다 버퍼에 저장하고 uboHandle에 복사한 후 GPU로
    memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat)); // InnerColor
    memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat)); // OuterColor
    memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));   // RadiusInner
    memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));   // RadiusOuter

    //ch10_5
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);                           // 버퍼 생성
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);            // 버퍼 바인딩
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW); // 데이터 복사

    gl->glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);


    //5. If linking is successful, install the program into the OpenGL pipeline.
    glUseProgram(programHandle);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

}

void ch1_10_UsingUniformBlocks::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programHandle);

    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    VAO.release();
}

const char *ch1_10_UsingUniformBlocks::loadShaderAsString(const QString &filePath)
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
