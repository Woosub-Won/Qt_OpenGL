#ifndef ch4_2_ApplyingA2DTexture_H
#define ch4_2_ApplyingA2DTexture_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

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
    void generateTorus(std::vector<float>& positions, std::vector<float>& normals, float outerRadius, float innerRadius, int numSides, int numRings);
private:

    std::vector<float> torusTexCoords;
    QOpenGLBuffer pVBO;  // Vertex Buffer Object for position
    QOpenGLBuffer nVBO;  // Vertex Buffer Object for normals
    QOpenGLBuffer texVBO;
    QOpenGLVertexArrayObject VAO;  // Vertex Array Object
    GLuint programHandle;

    std::vector<float> torusPositions;  // 토러스의 정점 데이터
    std::vector<float> torusNormals;    // 토러스의 법선 데이터

    const char* loadShaderAsString(const QString &filePath);
    GLuint LoadTexture(const char* texName, GLuint programHandle, const char* uniformName, QOpenGLFunctions* f);

};


#endif // ch4_2_ApplyingA2DTexture_H
