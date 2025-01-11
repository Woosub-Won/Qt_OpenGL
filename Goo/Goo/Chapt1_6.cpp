#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QOffscreenSurface>
#include <QFile>
#include <QFileInfo>
#include <QOpenGLWidget>

void Chapt1_6(){

    QOffscreenSurface offscreenSurface;
    offscreenSurface.create();

    QOpenGLContext glContext;
    glContext.setFormat(offscreenSurface.format());
    glContext.create();
    glContext.makeCurrent(&offscreenSurface);
    QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();
    // QOpenGLFunctions* glFuncs=widget->context()->functions();

    QOpenGLShader vertShader(QOpenGLShader::Vertex);

    qDebug() << "Shader file path:" << QFileInfo("shader/basic.vert").absoluteFilePath();
    QFile vShaderFile("shader/basic.vert");
    if (!QFile::exists("shader/basic.vert")) {
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

    qDebug() << "Shader file path:" << QFileInfo("shader/basic.frag").absoluteFilePath();
    QFile fShaderFile("shader/basic.frag");
    if(!QFile::exists("shader/basic.frag")){
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

    QOpenGLShaderProgram shaderProgram;

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

    if(!shaderProgram.link()){
        qDebug() << "Failed to link shader program";
        qDebug() << shaderProgram.log();
        return;
    }
    shaderProgram.bind();
}
