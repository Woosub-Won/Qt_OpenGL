#ifndef CH1_7_SENDINGDATATOSHADER_H
#define CH1_7_SENDINGDATATOSHADER_H

#include <QOpenGLWidget>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>


class Ch1_7_SendingDataToShader : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Ch1_7_SendingDataToShader(QWidget* parent = nullptr);
    ~Ch1_7_SendingDataToShader();

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

#endif // CH1_7_SENDINGDATATOSHADER_H
