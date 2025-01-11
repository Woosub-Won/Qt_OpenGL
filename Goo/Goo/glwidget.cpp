#include "glwidget.h"

GLWidget::GLWidget(const QString& vertexPath, const QString& fragmentPath, QVector<float>& positionData, QVector<float>& colorData)
    : vertexPath(vertexPath),    // 멤버 변수 초기화
    fragmentPath(fragmentPath),
    positionData(positionData),
    colorData(colorData) {}

void GLWidget::initializeGL(){
    initializeOpenGLFunctions();


    QSurfaceFormat format;
    format.setVersion(4,0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QOffscreenSurface offscreenSurface;
    offscreenSurface.setFormat(format);
    offscreenSurface.create();

    this->setFormat(format);
    QOpenGLContext glContext;
    glContext.setFormat(format);
    glContext.create();
    glContext.makeCurrent(&offscreenSurface);

    glFuncs = QOpenGLContext::currentContext()->functions();
    if(glFuncs==nullptr){
        qDebug() << "glFuncs is nullPtr";
        return;
    }

    QOpenGLShader vertShader(QOpenGLShader::Vertex);

    qDebug() << "Shader file path:" << QFileInfo(vertexPath).absoluteFilePath();
    QFile vShaderFile(vertexPath);
    if (!QFile::exists(vertexPath)) {
        qDebug() << "Shader file does not exist!";
        return;
    }
    if(!vShaderFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Failed to open shader file!";
        return;
    }
    QByteArray shaderCode = vShaderFile.readAll();

    vertShader.compileSourceCode(shaderCode);
    if(!vertShader.isCompiled()){
        qDebug() << "Vertex shader compilation failed!";
        qDebug() << vertShader.log();
    }
    else{
        qDebug() << "Vertex shader compiled successfully";
    }

    QOpenGLShader fragShader(QOpenGLShader::Fragment);

    qDebug() << "Shader file path:" << QFileInfo(fragmentPath).absoluteFilePath();
    QFile fShaderFile(fragmentPath);
    if(!QFile::exists(fragmentPath)){
        qDebug() << "Shader file does not exist";
        return;
    }
    if(!fShaderFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Failed to open shader file";
        return;
    }
    shaderCode = fShaderFile.readAll();
    fragShader.compileSourceCode(shaderCode);
    if(!fragShader.isCompiled()){
        qDebug() << "Fragment shader compilation failed";
        qDebug() << fragShader.log();
    }
    else{
        qDebug() << "Fragment shader compiled successfully";
    }

    if(!shaderProgram.addShader(&vertShader)){
        qDebug() << "Failed to add vertex shader to program";
        qDebug() << shaderProgram.log();

        return;
    }
    if(!shaderProgram.addShader(&fragShader)){
        qDebug() << "Failed to add fragment shader to program";
        qDebug() << shaderProgram.log();

        return;
    }
    shaderProgram.bindAttributeLocation("VertePosition", 0);
    shaderProgram.bindAttributeLocation("VertexColor", 1);

    if(!shaderProgram.link()){
        qDebug() << "Failed to link shader program";
        qDebug() << shaderProgram.log();
        return;
    }
    shaderProgram.bind();

    vao.create();
    if(!vao.isCreated()){
        qDebug() <<"vao is not created in initializeGL";
    }
    vao.bind();

    GLuint vboHandles[2];
    glFuncs->glGenBuffers(2, vboHandles);

    GLuint positionBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];

    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glFuncs->glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), positionData.data(), GL_STATIC_DRAW);

    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glFuncs->glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), colorData.data(), GL_STATIC_DRAW);

    glFuncs->glEnableVertexAttribArray(0);
    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glFuncs->glEnableVertexAttribArray(1);
    glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    vao.release();
}

void GLWidget::resizeGL(int w, int h){
    if(glFuncs==nullptr){
        qDebug() <<"glFuncs is nullptr";
    }
    qDebug() << "Current context:" << QOpenGLContext::currentContext();

    glFuncs->glViewport(0,0,w,h);


}
void GLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram.bind();
    if(!vao.isCreated()){
        qDebug() <<"vao is not created";
    }
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shaderProgram.release();
    vao.release();

}
