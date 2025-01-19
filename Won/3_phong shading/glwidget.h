#pragma once

#include <QOpenGLWidget.h>
#include <QOpenGLFunctions.h>
#include <QTimer.h>
#include <QOpenGLShaderProgram.h>
#include <QOpenGLBuffer.h>
#include <QOpenGLVertexArrayObject.h>
#include <QMatrix4x4.h>
#include <QOpenGLFunctions_4_0_Core.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

const char* const vertexShader = R"glsl(
    #version 400

    in vec3 VertexPosition;
    in vec3 VertexNormal;
    
    out vec3 Position;
    out vec3 Normal;

    uniform mat4 ModelViewMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 ProjectionMatrix;
    uniform mat4 MVP;

    void main() 
    {
        Normal = normalize(NormalMatrix * VertexNormal);
        Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
        
        gl_Position = MVP * vec4(VertexPosition, 1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
    #version 400
    
    in vec3 Position;
    in vec3 Normal;

    uniform vec4 LightPosition;
    uniform vec3 LightIntensity;
    uniform vec3 Kd;
    uniform vec3 Ka;
    uniform vec3 Ks;
    uniform float Shininess;

    out vec4 FragColor;
    
    vec3 ads()
    {
        vec3 n = normalize(Normal);
        vec3 s = normalize( vec3(LightPosition) - Position );
        vec3 v = normalize(vec3(-Position));
        vec3 r = reflect( -s, n );
        return  LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ) +
                Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
    }

    void main() 
    {
        FragColor = vec4(ads(), 1.0);
    }
)glsl";

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    ~GLWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void readObjFile(const char* fileName);

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoord;
    std::vector<int> indices;

    float rotationAngle;

    GLuint adsIndex;
    GLuint diffuseIndex;

private:
    QTimer timer;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer positionBuffer;
    QOpenGLBuffer normalBuffer;
    QOpenGLBuffer texBuffer;
    QOpenGLBuffer indexBuffer;

    QMatrix4x4 modelViewMatrix;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 mvpMatrix;

    // 라이트 관련 변수들
    QVector4D lightPosition;
    QVector3D lightIntensity;

    QVector3D lightAmbient;
    QVector3D lightDiffuse;
    QVector3D lightSpecular;

    // 재질 관련 변수들
    QVector3D materialAmbient;
    QVector3D materialDiffuse;
    QVector3D materialSpecular;
    float materialShininess;



private slots:
    void updateGL() {
        update();
    }
};
