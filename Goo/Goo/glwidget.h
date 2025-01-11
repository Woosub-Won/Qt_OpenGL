#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QDebug>

class GLWidget:public QOpenGLWidget, protected QOpenGLFunctions
{
protected:
    void initializeGL() override;
public:
    GLWidget();
};

#endif // GLWIDGET_H
