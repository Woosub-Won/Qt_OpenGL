#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>

#include "shader.h"
using namespace std;
using glm::mat4;
using glm::vec3;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "window", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
	}
	Shader shader = Shader("shader/Chapt1_9.vert", "shader/Chapt1_9.frag");
	shader.use();
	glBindAttribLocation(shader.ID, 0, "VertexPosition");
	glBindAttribLocation(shader.ID, 1, "VertexColor");

	GLuint vaoHandle;

	float positionData[] = {
	 -0.8f, -0.8f, 0.0f,
	  0.8f, -0.8f, 0.0f,
	  0.0f,  0.8f, 0.0f };
	float colorData[] = {
	  1.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 1.0f };

	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	GLint maxLength, nAttribs;
	glGetProgramiv(shader.ID, GL_ACTIVE_ATTRIBUTES, &nAttribs);
	glGetProgramiv(shader.ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

	GLchar* name = (GLchar*)malloc(maxLength);

	GLint written, size, location;
	GLenum type;

	free(name);
	float angle = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		angle += 0.05f;
		mat4 rotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f));
		
		GLuint location = glGetUniformLocation(shader.ID, "RotationMatrix");
		if (location >= 0) {
			glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
		}
		
		glBindVertexArray(vaoHandle);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}