#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QOffscreenSurface>
#include <QFile>
#include <QFileInfo>
void Chapt1_5(){
    QOffscreenSurface offscreenSurface;
    offscreenSurface.create();

    QOpenGLContext glContext;
    glContext.setFormat(offscreenSurface.format());
    glContext.create();
    glContext.makeCurrent(&offscreenSurface);
    QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();

    QOpenGLShader vertShader(QOpenGLShader::Vertex);

    qDebug() << "Shader file path:" << QFileInfo("shader/basic.vert").absoluteFilePath();
    QFile shaderFile("shader/basic.vert");
    if (!QFile::exists("shader/basic.vert")) {
        qDebug() << "Shader file does not exist!";
        return;
    }
    if(!shaderFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Failed to open shader file!";
        return;
    }
    QByteArray shaderCode = shaderFile.readAll();

    vertShader.compileSourceCode(shaderCode);
    if(!vertShader.isCompiled()){
        qDebug() << "Vertex shader compilation failed!";
        qDebug() << vertShader.log();
    }
    else{
        qDebug() << "Vertex shader compiled successfully";
    }
}
