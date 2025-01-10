#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QOffscreenSurface>

void Chapt1_4(){
    QOffscreenSurface offscreenSurface;
    offscreenSurface.create();

    QOpenGLContext glContext;
    glContext.setFormat(offscreenSurface.format());
    glContext.create();
    glContext.makeCurrent(&offscreenSurface);
    QOpenGLFunctions* glFuncs = glContext.functions();

    const GLubyte* renderer = glFuncs->glGetString(GL_RENDERER);
    const GLubyte* vendor = glFuncs->glGetString(GL_VENDOR);
    const GLubyte* version = glFuncs->glGetString(GL_VERSION);
    const GLubyte* glslVersion = glFuncs->glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLint major, minor;
    glFuncs->glGetIntegerv(GL_MAJOR_VERSION, &major);
    glFuncs->glGetIntegerv(GL_MINOR_VERSION, &minor);

    qDebug() << "GL Vendor    :" << vendor;
    qDebug() << "GL Renderer  :" << renderer;
    qDebug() << "GL Version (string)  :" << version;
    qDebug() << "GL Version (integer) :" << major << "." << minor;
    qDebug() << "GLSL Version :" << glslVersion;

}
