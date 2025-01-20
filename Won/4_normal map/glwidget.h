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
    in vec4 VertexTangent;

    struct LightInfo
    {
        vec4 Position;
        vec3 Intensity;
    };
    uniform LightInfo Light;
    
    out vec3 LightDir;
    out vec2 TexCoord;
    out vec3 ViewDir;

    uniform mat4 ModelViewMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 ProjectionMatrix;
    uniform mat4 MVP;

    void main() 
    {
        // Transform normal and tangent to eye space
        vec3 norm = normalize(NormalMatrix * VertexNormal);
        vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));
        // Compute the binormal
        vec3 binormal = normalize( cross( norm, tang ) ) * VertexTangent.w;
        // Matrix for transformation to tangent space
        mat3 toObjectLocal = mat3(
            tang.x, binormal.x, norm.x,
            tang.y, binormal.y, norm.y,
            tang.z, binormal.z, norm.z ) ;
        // Get the position in eye coordinates
        vec3 pos = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
        // Transform light dir. and view dir. to tangent space
        LightDir = normalize( (toObjectLocal) * (Light.Position.xyz - pos) );
        ViewDir = toObjectLocal * normalize(-pos);
        // Pass along the texture coordinate
        TexCoord = VertexTexCoord;
        gl_Position = MVP * vec4(VertexPosition,1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
    #version 400
    
    in vec3 LightDir;
    in vec2 TexCoord;
    in vec3 ViewDir;

    uniform sampler2D ColorTex;
    uniform sampler2D NormalMapTex;

    struct LightInfo
    {
        vec4 Position;
        vec3 Intensity;
    };
    uniform LightInfo Light;

    struct MaterialInfo
    {
        vec3 Ka;
        vec3 Ks;
        float Shininess;
    };
    uniform MaterialInfo Material;

    out vec4 FragColor;
    
    vec3 phongModel( vec3 norm, vec3 diffR ) 
    {
        vec3 r = reflect( -LightDir, norm );
        vec3 ambient = Light.Intensity * Material.Ka;
        float sDotN = max( dot(LightDir, norm), 0.0 );
        vec3 diffuse = Light.Intensity * diffR * sDotN;
        vec3 spec = vec3(0.0);
        if( sDotN > 0.0 )
            spec = Light.Intensity * Material.Ks * pow( max( dot(r,ViewDir), 0.0 ), Material.Shininess );
        
        return ambient + diffuse + spec;
    }

    void main() 
    {
        // Lookup the normal from the normal map
        vec3 normal = normalize(2.0 * texture( NormalMapTex, TexCoord ).xyz - 1.0);
        
        // The color texture is used as the diffuse reflectivity
        vec3 texColor = texture( ColorTex, TexCoord ).rgb;
        FragColor = vec4( phongModel(normal, texColor.rgb), 1.0 );
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
    std::vector<float> tangents; // 탄젠트 벡터를 저장할 벡터

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
    QOpenGLBuffer tangentBuffer; // 탄젠트 버퍼
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