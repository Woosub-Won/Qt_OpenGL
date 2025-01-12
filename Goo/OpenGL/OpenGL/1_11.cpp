#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
using namespace std;
using glm::mat4;
using glm::vec3;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

GLuint createTexture(const char*);

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
	Shader shader = Shader("shader/Chapt1_11.vert", "shader/Chapt1_11.frag");
	shader.use();
	

	GLuint vaoHandle;

	float positionData[] = {
	  0.5f,  0.5f, 0.0f,
	  0.5f, -0.5f, 0.0f,
	 -0.5f, -0.5f, 0.0f,
	 -0.5f,  0.5f, 0.0f,
	};
	float colorData[] = {
	  1.0f, 1.0f, 0.0f,
	  1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f,
	  0.0f, 1.0f, 0.0f,
	};
	unsigned int eIndices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];
	GLuint eboHandle;
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);

	glGenBuffers(1, &eboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eIndices), eIndices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glEnableVertexAttribArray(0);
	glBindAttribLocation(shader.ID, 0, "VertexPosition");
	glEnableVertexAttribArray(1);
	glBindAttribLocation(shader.ID, 1, "VertexTexCoord");

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	GLint nUniforms, maxLen;
	glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORMS, &nUniforms);

	GLuint blockIndex = glGetUniformBlockIndex(shader.ID, "BlobSettings");
	GLint blockSize;
	glGetActiveUniformBlockiv(shader.ID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);

	const GLchar* names[] = { "InnerColor", "OuterColor", "RadiusInner", "RadiusOuter" };
	GLuint indices[4];
	glGetUniformIndices(shader.ID, 4, names, indices);
	GLint offset[4];
	glGetActiveUniformsiv(shader.ID, 4, indices, GL_UNIFORM_OFFSET, offset);

	GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
	GLfloat innerRadius = 0.25f, outerRadius = 0.45f;

	memcpy(blockBuffer + offset[0], innerColor, sizeof(outerColor));
	memcpy(blockBuffer + offset[1], outerColor, sizeof(innerColor));
	memcpy(blockBuffer + offset[2], &innerRadius, sizeof(innerRadius));
	memcpy(blockBuffer + offset[3], &outerRadius, sizeof(outerRadius));

	GLuint uboHandle;
	glGenBuffers(1, &uboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

	free(blockBuffer);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vaoHandle);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

GLuint createTexture(const char* path) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture\n";
		return NULL;
	}
	stbi_image_free(data);
	return texture;
}