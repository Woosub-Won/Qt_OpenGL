#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

using namespace std;
 

int main()
{
	// openGL init
	glfwInit();
	
	// verion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat  vertices[] = { 
		-0.5f,		-0.5f * float(sqrt(3)) / 3,		0.0f, 1.0f, 0.0f, 0.0f,// Lower left corner
		0.5f,		-0.5f * float(sqrt(3)) / 3,		0.0f, 0.0f, 1.0f, 0.0f,// Lower right corner
		0.0f,		0.5f * float(sqrt(3)) * 2 / 3,	0.0f, 0.0f, 0.0f, 1.0f,// Upper corner
		-0.5f / 2,	0.5f * float(sqrt(3)) / 6,		0.0f, 0.0f, 0.0f, 0.0f, // Inner left
		0.5f / 2,	0.5f * float(sqrt(3)) / 6,		0.0f, 0.0f, 0.0f, 0.0f,// Inner right
		0.0f,		-0.5f * float(sqrt(3)) / 3,		0.0f, 0.0f, 0.0f, 0.0f// Inner down
	};

	GLuint indices[] =
	{
		0, 3, 5,
		3, 2, 4,
		5, 4, 1
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

	vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	
	vao1.Unbind();
	vbo1.Unbind();
	ebo1.Unbind();
	  
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shaderProgram.Activate();
		glUniform1f(uniID, 0.5f);

		vao1.Bind();

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	 
	vao1.Delete();
	vbo1.Delete();
	ebo1.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);    
	glfwTerminate();
	return 0;
}