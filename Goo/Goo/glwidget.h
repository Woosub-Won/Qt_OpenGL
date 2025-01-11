#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QOffscreenSurface>
#include <QFile>
#include <QFileInfo>
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QString>

class GLWidget:public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    // QOpenGLWidget* widget;
    QOpenGLFunctions* glFuncs;
    QOpenGLVertexArrayObject vao;
    QOpenGLShaderProgram shaderProgram;
    QString vertexPath;
    QString fragmentPath;
    QVector<float>& positionData;
    QVector<float>& colorData;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
public:
    GLWidget(const QString&, const QString&, QVector<float>&, QVector<float>&);
};

#endif // GLWIDGET_H
