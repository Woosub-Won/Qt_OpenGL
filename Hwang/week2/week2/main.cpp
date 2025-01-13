#include <vgl.h>
#include <InitShader.h>
#include "MyObject.h"

#include <vec.h>
#include <mat.h>

MyObject obj;

GLuint program;
GLuint prog;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 600;
int winHeight = 500;

void myInit()
{
	obj.Init("teapot.obj");

	prog = InitShader("vShader.glsl", "fShader.glsl");
	program = InitShader("vPhong.glsl", "fPhong.glsl");
}

float g_Time = 0;

void display()
{
	glClearColor(0.8, 0.8, 0.8, 1);   // 배경색 세팅

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 ViewMat = LookAt(vec3(3, 1, 2), vec3(0, 0, 0), vec3(0, 1, 0));

	float aspect = winWidth / (float)winHeight;
	float h = 1;

	mat4 ProjMat = Perspective(40, aspect, 0.1, 10);

	g_Mat = ProjMat * ViewMat;

	glUseProgram(prog);
	uMat = glGetUniformLocation(prog, "uMat");
	uColor = glGetUniformLocation(prog, "uColor");

	glUseProgram(program);
	GLuint uProjMat = glGetUniformLocation(program, "uProjMat");
	GLuint uModelMat = glGetUniformLocation(program, "uModelMat");

	mat4 ModelMat = RotateY(g_Time * 90);
	glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
	glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat * ModelMat);

	obj.Draw(program);

	glutSwapBuffers();
}

void idle()
{
	g_Time += 0.016;
	Sleep(16);					// for vSync
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
