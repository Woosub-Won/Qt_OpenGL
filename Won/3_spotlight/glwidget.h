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

    struct SpotLightInfo 
    {
        vec4 position; // Position in eye coords.
        vec3 intensity; // Amb., Diff., and Specular intensity
        vec3 direction; // Normalized direction of the spotlight
        float exponent; // Angular attenuation exponent
        float cutoff; // Cutoff angle (between 0 and 90)
    };
    uniform SpotLightInfo Spot;

    uniform vec3 Kd;
    uniform vec3 Ka;
    uniform vec3 Ks;
    uniform float Shininess;

    out vec4 FragColor;
    
    vec3 adsWithSpotLight()
    {
        vec3 s = normalize(vec3(Spot.position) - Position);
        float angle = acos(dot(-s, normalize(Spot.direction)));
        float cutoff = radians(clamp(Spot.cutoff, 0.0, 90.0));
        vec3 ambient = Spot.intensity * Ka;
        
        if(angle < cutoff) 
        {
            float spotFactor = pow(dot(-s, Spot.direction), Spot.exponent);
            vec3 v = normalize(vec3(-Position));
            vec3 h = normalize(v + s);
            return ambient + spotFactor * Spot.intensity * (
                Kd * max(dot(s, Normal), 0.0) +
                Ks * pow(max(dot(h,Normal), 0.0), Shininess)
            );
        }
        else 
        {
            return ambient;
        }
    }

    void main() 
    {
        FragColor = vec4(adsWithSpotLight(), 1.0);
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
    QVector3D lightDirection;
    float lightExponent;
    float cutoff;

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