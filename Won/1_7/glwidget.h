#pragma once

#include <QOpenGLWidget.h>
#include <QOpenGLFunctions.h>
#include <QTimer.h>
#include <QOpenGLShaderProgram.h>
#include <QOpenGLBuffer.h>
#include <QOpenGLVertexArrayObject.h>
#include <QMatrix4x4.h>

const char* const vertexShader = R"glsl(
	#version 400

    in vec3 VertexPosition;
    in vec3 VertexColor;

    out vec3 Color;

    uniform mat4 RotationMatrix;
    
    void main() {
        Color = VertexColor;
        gl_Position = RotationMatrix * vec4(VertexPosition, 1.0);
    }
)glsl";

const char* const fragmentShader = R"glsl(
	#version 400
    in vec3 Color;
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(Color, 1.0);
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

private:
    QTimer timer;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer positionBuffer;
    QOpenGLBuffer colorBuffer;
    QOpenGLBuffer indexBuffer;
    QMatrix4x4 rotationMatrix;
	

private slots:
	void updateGL() {
		update();
	}
};
