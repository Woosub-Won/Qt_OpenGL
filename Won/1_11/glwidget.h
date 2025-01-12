#pragma once

#include <QOpenGLWidget.h>
#include <QOpenGLFunctions.h>
#include <QTimer.h>
#include <QOpenGLShaderProgram.h>
#include <QOpenGLBuffer.h>
#include <QOpenGLVertexArrayObject.h>
#include <QMatrix4x4.h>
#include <QOpenGLExtraFunctions.h>

const char* const vertexShader = R"glsl(
	#version 400

    in vec3 VertexPosition;
    in vec3 VertexColor;

    out vec3 TexCoord;

    uniform mat4 RotationMatrix;
    
    void main() {
        TexCoord = VertexColor;
        gl_Position = RotationMatrix * vec4(VertexPosition, 1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
	#version 400
    in vec3 TexCoord;
    out vec4 FragColor;
    
    uniform BlobSettings {
        vec4 InnerColor;
        vec4 OuterColor;
        float RadiusInner;
        float RadiusOuter;
    } blob;

    void main() {
        float dx = TexCoord.x - 0.5;
        float dy = TexCoord.y - 0.5;
        float dist = sqrt(dx * dx + dy * dy);
        FragColor = mix(blob.InnerColor, blob.OuterColor, 
                smoothstep(blob.RadiusInner, blob.RadiusOuter, dist));
    }

)glsl";

class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
	Q_OBJECT
public:
	explicit GLWidget(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    ~GLWidget();

	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

    struct UniformBlock {
        QVector4D innerColor;
        QVector4D outerColor;
        float innerRadius;
        float outerRadius;
    };
private:
    QTimer timer;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer positionBuffer;
    QOpenGLBuffer colorBuffer;
    QOpenGLBuffer indexBuffer;

    QOpenGLBuffer uniformBuffer;
    UniformBlock uniformData;

    QMatrix4x4 rotationMatrix;
	

private slots:
	void updateGL() {
		update();
	}
};
