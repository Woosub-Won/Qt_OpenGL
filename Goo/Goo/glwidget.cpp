#include "glwidget.h"

GLWidget::GLWidget() {

}
void GLWidget::initializeGL(){
    initializeOpenGLFunctions();
    if(!context()||!isValid()){
        qCritical() << "Error initializing OpenGL context";
        return;
    }
    qDebug() << "OpenGL initialized successfully";
}
