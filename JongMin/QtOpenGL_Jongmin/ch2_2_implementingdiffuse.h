#ifndef CH2_2_IMPLEMENTINGDIFFUSE_H
#define CH2_2_IMPLEMENTINGDIFFUSE_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class ch2_2_ImplementingDiffuse : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ch2_2_ImplementingDiffuse(QWidget* parent = nullptr):QOpenGLWidget(parent){

    };
    ~ch2_2_ImplementingDiffuse(){

    }

protected:
    void initializeGL();
    void paintGL();
    void generateTorus(std::vector<float>& positions, std::vector<float>& normals, float outerRadius, float innerRadius, int numSides, int numRings);
private:

    QOpenGLBuffer pVBO;  // Vertex Buffer Object for position
    QOpenGLBuffer nVBO;  // Vertex Buffer Object for normals
    QOpenGLVertexArrayObject VAO;  // Vertex Array Object
    GLuint programHandle;

    std::vector<float> torusPositions;  // 토러스의 정점 데이터
    std::vector<float> torusNormals;    // 토러스의 법선 데이터

    const char* loadShaderAsString(const QString &filePath);

};


#endif // CH2_2_IMPLEMENTINGDIFFUSE_H
