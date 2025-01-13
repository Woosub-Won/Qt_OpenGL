#pragma once

#include <QOpenGLWidget.h>
#include <QOpenGLFunctions.h>
#include <QTimer.h>
#include <QOpenGLShaderProgram.h>
#include <QOpenGLBuffer.h>
#include <QOpenGLVertexArrayObject.h>
#include <QMatrix4x4.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

const char* const vertexShader = R"glsl(
    #version 400
    
    in vec3 VertexPosition;
    in vec3 VertexNormal;
    
    out vec3 LightIntensity;
    
    struct LightInfo {
        vec4 Position;      // Light position in eye coords
        vec3 La;            // Ambient light intensity
        vec3 Ld;            // Diffuse light intensity
        vec3 Ls;            // Specular light intensity
    };
    uniform LightInfo Light;
    
    struct MaterialInfo {
        vec3 Ka;            // Ambient reflectivity
        vec3 Kd;            // Diffuse reflectivity
        vec3 Ks;            // Specular reflectivity
        float Shininess;    // Specular shininess factor
    };
    uniform MaterialInfo Material;
    
    uniform mat4 ModelViewMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 ProjectionMatrix;
    uniform mat4 MVP;
    
    void main() {
        vec3 tnorm = normalize(NormalMatrix * VertexNormal);
        vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
        vec3 s = normalize(vec3(Light.Position - eyeCoords));
        vec3 v = normalize(-eyeCoords.xyz);
        vec3 r = reflect(-s, tnorm);
        
        // 주변광(Ambient)
        vec3 ambient = Light.La * Material.Ka;
        
        // 분산광(Diffuse)
        float sDotN = max(dot(s, tnorm), 0.0);
        vec3 diffuse = Light.Ld * Material.Kd * sDotN;
        
        // 반사광(Specular)
        vec3 spec = vec3(0.0);
        if(sDotN > 0.0)
            spec = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
        
        LightIntensity = ambient + diffuse + spec;
        gl_Position = MVP * vec4(VertexPosition, 1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
    #version 400
    
    in vec3 LightIntensity;
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(LightIntensity, 1.0);
    }
)glsl";

class GLWidget : public QOpenGLWidget, public QOpenGLFunctions
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
    std::vector<int> indices;

    float rotationAngle;

private:
    QTimer timer;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer positionBuffer;
    QOpenGLBuffer colorBuffer;
    QOpenGLBuffer indexBuffer;

    QMatrix4x4 modelViewMatrix;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 mvpMatrix;

    // 라이트 관련 변수들
    QVector4D lightPosition;
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
