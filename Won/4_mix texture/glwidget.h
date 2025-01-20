#pragma once

#include <QOpenGLWidget.h>
#include <QOpenGLFunctions.h>
#include <QTimer.h>
#include <QOpenGLShaderProgram.h>
#include <QOpenGLBuffer.h>
#include <QOpenGLTexture.h>
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
    in vec2 VertexTexCoord;
    
    out vec3 Position;
    out vec3 Normal;
    out vec2 TexCoord;

    uniform mat4 ModelViewMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 ProjectionMatrix;
    uniform mat4 MVP;

    void main() 
    {
        TexCoord = VertexTexCoord;
        Normal = normalize(NormalMatrix * VertexNormal);
        Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
        
        gl_Position = MVP * vec4(VertexPosition, 1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
    #version 400
    
    in vec3 Position;
    in vec3 Normal;
    in vec2 TexCoord;

    uniform sampler2D Tex1;
    uniform sampler2D Tex2;

    struct LightInfo
    {
        vec4 Position;
        vec3 Intensity;
    };
    uniform LightInfo Light;

    struct MaterialInfo
    {
        vec3 Ka;
        vec3 Kd;
        vec3 Ks;
        float Shininess;
    };
    uniform MaterialInfo Material;

    out vec4 FragColor;
    
    void phongModel( vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec ) 
    {
        vec3 n = normalize(norm);
        vec3 s = normalize(vec3(Light.Position) - pos); // Light direction
        vec3 v = normalize(-pos); // View direction (eye is at origin)
        vec3 r = reflect(-s, n); // Reflected light direction

        // Ambient component
        vec3 ambient = Light.Intensity * Material.Ka;

        // Diffuse component
        float sDotN = max(dot(s, n), 0.0);
        vec3 diffuse = Light.Intensity * Material.Kd * sDotN;

        // Specular component
        vec3 specular = vec3(0.0);
        if (sDotN > 0.0) {
            specular = Light.Intensity * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
        }

        ambAndDiff = ambient + diffuse;
        spec = specular;
    }

    void main() 
    {
        vec3 ambAndDiff, spec;
        vec4 brickTexColor = texture(Tex1, TexCoord);
        vec4 mossTexColor = texture(Tex2, TexCoord);
        phongModel(Position, Normal, ambAndDiff, spec);
        vec4 texColor = mix(brickTexColor, mossTexColor, 0.7);
        FragColor = vec4(ambAndDiff, 1.0) * texColor + vec4(spec, 1.0);
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
    QOpenGLTexture* texture1;
    QOpenGLTexture* texture2;

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