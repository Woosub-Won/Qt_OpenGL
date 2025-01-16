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
	Shader shader = Shader("shader/Chapt1_11(2).vert", "shader/Chapt1_11(2).frag");
	shader.use();

	float vertices[] = {
	  0.5f,  0.5f, 0.0f,1.0f, 1.0f, 0.0f,
	  0.5f, -0.5f, 0.0f,1.0f, 0.0f, 0.0f,
	 -0.5f, -0.5f, 0.0f,0.0f, 0.0f, 0.0f,
	 -0.5f,  0.5f, 0.0f,0.0f, 1.0f, 0.0f,
	};
	GLuint eIndices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eIndices), eIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLint nUniforms, maxLen;
	glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORMS, &nUniforms);

	// ������ ����� �ε����� ��ȯ�ϴ� �Լ�
	// �Ű�����
	// 1. ���̴� ���α׷��� ���� �ĺ���
	// 2. ������ ����� �̸�
	// ���⼭ ���ϴ� �ε����� ������ �ĺ��ڸ� �ǹ���
	GLuint blockIndex = glGetUniformBlockIndex(shader.ID, "BlobSettings");
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "Uniform block not found!" << std::endl;
		return 0;
	}
	GLint blockSize;
	// Ȱ��ȭ�� ���̴� ���α׷��� ������ ��Ͽ� ���� ������ ��ȸ�� �� �ְ� �ϴ� �Լ�
	// �Ű�����
	// 1. ���̴� ���α׷��� ���� �ĺ���
	// 2. ������ ����� �ε���
	// 3. ��ȸ�ϰ��� �ϴ� ������ ��� �Ӽ��� ����
	// 4. ��ȸ�� �Ӽ��� ������ �迭 ������
	glGetActiveUniformBlockiv(shader.ID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);
	const GLchar* names[] = { "BlobSettings.InnerColor", "BlobSettings.OuterColor", "BlobSettings.RadiusInner", "BlobSettings.RadiusOuter" };
	GLuint indices[4];

	// ������ ��Ͼȿ� ���� �ִ� ���������� �ε����� ��µ� ���� �Լ�
	// �Ű�����
	// 1. ���̴� ���α׷��� �ĺ���
	// 2. ������ ����� �ε��� (����� 1�� �� �� ���� ����)
	// 3. ��ȸ�Ϸ��� �������� ��
	// 4. ������ �̸��� �迭 ������
	// 5. �� �������� ���� �ε����� ������ �迭 ������
	glGetUniformIndices(shader.ID, 4, names, indices);
	GLint offset[4];

	// �ټ��� ������ Ȥ�� ������ ����� �Ӽ��� ��ȸ�ϴµ� ���
	// �Ű�����
	// 1. ���̴� ���α׷��� �ĺ���
	// 2. ��ȸ�Ϸ��� �������� ����
	// 3. �������� �ε��� �迭
	// 4. ��ȸ�ϰ��� �ϴ� �������� �Ӽ� ����
	// 5. ��ȸ�� �Ӽ��� ������ �迭 ������
	glGetActiveUniformsiv(shader.ID, 4, indices, GL_UNIFORM_OFFSET, offset);

	GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
	GLfloat innerRadius = 0.25f, outerRadius = 0.45f;



	// ������ ���� �޸� ����� ������ �޸� ������� �����ϴ� �Լ�
	// �Ű�����
	// 1. ������ �޸��� ������
	// 2. ���� �޸��� ������
	// 3. ������ ����Ʈ ��
	memcpy(blockBuffer + 0, innerColor, sizeof(innerColor));
	memcpy(blockBuffer + 16, outerColor, sizeof(outerColor));
	memcpy(blockBuffer + 32, &innerRadius, sizeof(innerRadius));
	memcpy(blockBuffer + 36, &outerRadius, sizeof(outerRadius));

	GLuint UBO;
	glGenBuffers(1, &UBO);

	// ������ ���� : ���̴� ���α׷��� �ʿ�� �ϴ� �����͸� ������ ������ �ѹ��� �����ϴ� ����
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, UBO);

	free(blockBuffer);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			cout << "OpenGL Error: " << error << endl;
		}

		glDrawElements(GL_TRIANGLES, sizeof(eIndices) / sizeof(eIndices[0]), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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