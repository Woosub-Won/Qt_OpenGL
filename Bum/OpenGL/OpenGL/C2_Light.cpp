#include <iostream> 
#include <stb/stb_image.h>

#include "OpenGL.h"

#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"

#include "Pyramid.h"

const float screenHeight = 800;
const float screenWidth = screenHeight * 1.6f;


using namespace std;


int main()
{
	// openGL init
	glfwInit();

	// verion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Vertices coordinates
	GLfloat vertices[] =
	{ //     COORDINATES     /        COLORS        /    TexCoord    /       NORMALS     //
		-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3
	}; 

	GLfloat lightVertices[] =
	{ //     COORDINATES     //
		-0.1f, -0.1f,  0.1f,
		-0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f,  0.1f
	};

	GLuint lightIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};
	//create window
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL,
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

	glViewport(0, 0, screenWidth, screenHeight);

	//Pyramid 
	Pyramid frogPyramid; 
	frogPyramid.Init();

	//Floor 
	Shader floorShader("default.vert", "default.frag");
	VAO floorVao;
	floorVao.Bind();
	
	VBO floorVbo(vertices, sizeof(vertices));
	EBO floorEbo(indices, sizeof(indices));

	floorVao.LinkAttrib(floorVbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	floorVao.LinkAttrib(floorVbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	floorVao.LinkAttrib(floorVbo, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	floorVao.LinkAttrib(floorVbo, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));

	floorVao.Unbind();
	floorVbo.Unbind();
	floorEbo.Unbind();


	// light
	Shader lightShader("light.vert", "light.frag");
	VAO lightVao;
	lightVao.Bind();

	VBO lightVbo(lightVertices, sizeof(lightVertices));
	EBO lightEbo(lightIndices, sizeof(lightIndices));

	lightVao.LinkAttrib(lightVbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	
	lightVao.Unbind();
	lightVbo.Unbind();
	lightEbo.Unbind();

	Camera camera(screenWidth, screenHeight, vec3(0.0f, 1.0f, 2.0f));

	// Uniform  
	vec3 lightPosition = vec3(0.5f, 1.5f, 0.5f);
	vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Pyradmid
	Shader frogShader = frogPyramid.GetShader();
	{
		frogShader.Activate();

		mat4 objModelMatrix = mat4(1.0f);
		objModelMatrix = translate(objModelMatrix, vec3(-2.0f, -0.2f, 0.0f));
		SetMatrixUniform(frogShader, "modelMatrix", objModelMatrix);
		SetVectorUniform(frogShader, "lightPos", lightPosition);
		SetVectorUniform(frogShader, "lightColor", lightColor);
		SetVectorUniform(frogShader, "cameraPos", camera.Position);

	}
	
	// Floor
	Texture floorTexture; 
	Texture floorSpecTexture;
	{
		floorShader.Activate();

		mat4 objModelMatrix = mat4(1.0f);
		objModelMatrix = translate(objModelMatrix, vec3(0.0f, 0.0f, 0.0f)) *scale(objModelMatrix, vec3(2.0f, 2.0f, 2.0f));
		SetMatrixUniform(floorShader, "modelMatrix", objModelMatrix);
		SetVectorUniform(floorShader, "lightPos", lightPosition);
		SetVectorUniform(floorShader, "lightColor", lightColor);
		SetVectorUniform(floorShader, "cameraPos", camera.Position);

		floorTexture.Init("planks.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
		floorTexture.texUnit(floorShader, "tex0", 0);
		 
		floorSpecTexture.Init("planksSpec.png", GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);
		floorSpecTexture.texUnit(floorShader, "tex1", 1);

	}

	// light
	{
		lightShader.Activate();
		mat4 lightModelMatrix = mat4(1.0f);
		lightModelMatrix = translate(lightModelMatrix, lightPosition);

		SetMatrixUniform(lightShader, "modelMatrix", lightModelMatrix);
		SetVectorUniform(lightShader, "lightColor", lightColor);
		 
	}

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// First : Do Activate Shader AND SETTIGN VALUES

		//camera 
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
		
		//Pyramid
		frogShader.Activate(); 
		camera.Matrix(frogShader, "cameraMatrix"); 
		frogPyramid.Draw();
		 
		//Floor 
		floorShader.Activate();
		camera.Matrix(floorShader, "cameraMatrix"); 

		floorVao.Bind();
		floorTexture.Bind(0);
		floorSpecTexture.Bind(1);

		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		// Light
		lightShader.Activate();
		camera.Matrix(lightShader, "cameraMatrix");
		lightVao.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//vao1.Delete();
	//vbo1.Delete();
	//ebo1.Delete();
	//texture.Delete();
	//shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}