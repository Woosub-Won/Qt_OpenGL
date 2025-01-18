#ifndef ch4_2_ApplyingA2DTexture_H
#define ch4_2_ApplyingA2DTexture_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include "objreader.h"

class ch4_2_ApplyingA2DTexture : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ch4_2_ApplyingA2DTexture(QWidget* parent = nullptr):QOpenGLWidget(parent){

    };
    ~ch4_2_ApplyingA2DTexture(){

    }

protected:
    void initializeGL();
    void paintGL();
    //void generateTorus(std::vector<float>& positions, std::vector<float>& normals, float outerRadius, float innerRadius, int numSides, int numRings);
private:

    objReader objreader;
    QOpenGLBuffer pVBO;  // Vertex Buffer Object for position
    QOpenGLBuffer nVBO;  // Vertex Buffer Object for normals
    QOpenGLBuffer texVBO;
    QOpenGLVertexArrayObject VAO;  // Vertex Array Object
    GLuint programHandle;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLTexture* texture;

    const char* loadShaderAsString(const QString &filePath);


};


#endif // ch4_2_ApplyingA2DTexture_H
