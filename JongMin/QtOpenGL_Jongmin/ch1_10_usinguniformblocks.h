#ifndef CH1_10_USINGUNIFORMBLOCKS_H
#define CH1_10_USINGUNIFORMBLOCKS_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QFile>
#include <QMatrix4x4>
#include <QOpenGLExtraFunctions>

class ch1_10_UsingUniformBlocks : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ch1_10_UsingUniformBlocks(QWidget* parent = nullptr):QOpenGLWidget(parent){

    };
    ~ch1_10_UsingUniformBlocks();

protected:
    void initializeGL();
    void paintGL();

private:

    //QOpenGLBuffer VBO;  // Vertex Buffer Object
    QOpenGLBuffer tVBO; //VBO for color
    QOpenGLBuffer pVBO;
    QOpenGLVertexArrayObject VAO;  // Vertex Array Object
    GLuint programHandle;
    const char* loadShaderAsString(const QString &filePath);

};

#endif // CH1_10_USINGUNIFORMBLOCKS_H
