#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QOffscreenSurface>
#include <QDebug>


void Chapt1_2(){

    QOffscreenSurface offscreenSurface;
    offscreenSurface.create();

    QOpenGLContext glContext;
    glContext.setFormat(offscreenSurface.format());
    glContext.create();
    glContext.makeCurrent(&offscreenSurface);

    QOpenGLFunctions* funcs = glContext.functions();

    GLenum err = funcs->glGetError();
    if(err != GL_NO_ERROR){
        qWarning() << "OpenGL Error: " << funcs->glGetError();
    }
}
// 자꾸 컨텍스트가 없다는데 모르겠음
