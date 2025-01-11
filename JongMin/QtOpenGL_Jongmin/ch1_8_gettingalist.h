#ifndef CH1_8_GETTINGALIST_H
#define CH1_8_GETTINGALIST_H

#include <QOpenGLWidget>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>


class ch1_8_GettingAList : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ch1_8_GettingAList(QWidget* parent = nullptr):QOpenGLWidget(parent) {

    }
    ~ch1_8_GettingAList();

protected:
    void initializeGL();
    void paintGL();

private:
    QOpenGLBuffer VBO;  // Vertex Buffer Object
    QOpenGLBuffer colorVBO; //VBO for color
    QOpenGLVertexArrayObject VAO;  // Vertex Array Object
    GLuint programHandle;
    const char* loadShaderAsString(const QString &filePath);

};

#endif // ch1_8_GettingAList_H
