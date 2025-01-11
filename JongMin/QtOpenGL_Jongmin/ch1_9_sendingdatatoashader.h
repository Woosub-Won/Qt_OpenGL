#ifndef CH1_9_SENDINGDATATOASHADER_H
#define CH1_9_SENDINGDATATOASHADER_H

#include <QOpenGLWidget>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>


class ch1_9_SendingDataToAShader : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ch1_9_SendingDataToAShader(QWidget* parent = nullptr):QOpenGLWidget(parent){

    };
    ~ch1_9_SendingDataToAShader();

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

#endif // CH1_9_SENDINGDATATOASHADER_H
