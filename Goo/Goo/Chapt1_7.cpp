#include "glwidget.h"

void renderLoop(QOpenGLFunctions*, QOpenGLWidget*);

void Chapt1_7(){
    QString vertexPath("shader/basic.vert");
    QString fragmentPath("shader/basic.frag");


    QVector<float> positionData = {
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f,  0.8f, 0.0f
    };
    QVector<float> colorData = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    GLWidget* widget= new GLWidget(vertexPath, fragmentPath, positionData, colorData);
    widget->show();

    delete widget;
}
