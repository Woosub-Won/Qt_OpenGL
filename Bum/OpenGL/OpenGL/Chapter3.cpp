#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"


using namespace std;


int main()
{
	// openGL init
	glfwInit();

	// verion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 0   1
	//      
	// 3   2 
	GLfloat  vertices[] = {
		-0.5, 0.5, 0,		1.0, 0.0, 0.0,	0,0,
		 0.5, 0.5, 0,		0.0, 1.0, 0.0,	1,0,
		 0.5, -0.5, 0,		0.0, 0.0, 1.0,	1,1,
		 -0.5, -0.5, 0,		0.0, 1.0, 0.0,	0,1,
	};

	GLuint indices[] =
	{
		0, 2, 1,
		2, 0, 3
	};

	//create window
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL", NULL,
		NULL);
	if (window == NULL)
	{
		cout << "Faild Create GLFW Window" << endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	//load glad
	gladLoadGL();

	glViewport(0, 0, 800, 800);

	Shader shaderProgram("default.vert", "default.frag");

	VAO vao1;
	vao1.Bind();

	VBO vbo1(vertices, sizeof(vertices));
	EBO ebo1(indices, sizeof(indices));

	vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao1.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	vao1.Unbind();
	vbo1.Unbind();
	ebo1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	Texture	texture("frog.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);
	texture.Unbind();
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		glUniform1f(uniID, 0.5f);
		texture.Bind();

		vao1.Bind();

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	vao1.Delete();
	vbo1.Delete();
	ebo1.Delete();
	texture.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}