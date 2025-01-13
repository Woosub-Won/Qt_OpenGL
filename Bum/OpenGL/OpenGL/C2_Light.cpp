#include <iostream> 
#include <stb/stb_image.h>

#include "OpenGL.h"

#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Mesh.h"
#include "MySphere.h"
#include "Light.h"
<<<<<<< HEAD

=======
#include"FogFactor.h"
>>>>>>> origin/bum
 

const float screenHeight = 800;
const float screenWidth = screenHeight * 1.6f;


using namespace std;

// Vertices coordinates
// 1  2 
// 0  3

Vertex vertices[] =
{ //      0         COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
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

// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
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

int main()
{
	// openGL init
	glfwInit();

	// verion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
<<<<<<< HEAD

	 
	//// Vertices coordinates
	//GLfloat vertices[] =
	//{ //     COORDINATES     /        COLORS        /    TexCoord    /       NORMALS     //
	//	-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	//	-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	//	 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	//	 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
	//};

	//// Indices for vertices order
	//GLuint indices[] =
	//{
	//	0, 1, 2,
	//	0, 2, 3
	//}; 

	//GLfloat lightVertices[] =
	//{ //     COORDINATES     //
	//	-0.1f, -0.1f,  0.1f,
	//	-0.1f, -0.1f, -0.1f,
	//	 0.1f, -0.1f, -0.1f,
	//	 0.1f, -0.1f,  0.1f,
	//	-0.1f,  0.1f,  0.1f,
	//	-0.1f,  0.1f, -0.1f,
	//	 0.1f,  0.1f, -0.1f,
	//	 0.1f,  0.1f,  0.1f
	//};

	//GLuint lightIndices[] =
	//{
	//	0, 1, 2,
	//	0, 2, 3,
	//	0, 4, 7,
	//	0, 7, 3,
	//	3, 7, 6,
	//	3, 6, 2,
	//	2, 6, 5,
	//	2, 5, 1,
	//	1, 5, 4,
	//	1, 4, 0,
	//	4, 5, 6,
	//	4, 6, 7
	//};
=======
	 
>>>>>>> origin/bum
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
	
	Texture textures[]
	{
		Texture("planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	
	//Floor 
	Shader shaderProgram("default.vert", "default.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	
	vec3 floorBasePos = vec3(0, 0, 0);
	Mesh floor;
	floor.Init(verts, ind, tex);
	
	//Sphere
	MySphere sphere;
	sphere.Init(30,30);
	 
	// light
	Shader lightShader("light.vert", "light.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, tex);

	Camera camera(screenWidth, screenHeight, vec3(0.0f, 1.0f, 2.0f));

	// Uniform  
	vec3 lightPosition = vec3(0.5f, 1.5f, 0.5f);
	vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	
<<<<<<< HEAD
=======
	//shaderProgram
>>>>>>> origin/bum
	// Floor 
	LightInfo lights[2];
	{
		shaderProgram.Activate();

		mat4 objModelMatrix = mat4(1.0f);
		objModelMatrix = translate(objModelMatrix, vec3(0.0f, 0.0f, 0.0f)) *scale(objModelMatrix, vec3(2.0f, 2.0f, 2.0f));
		SetMatrixUniform(shaderProgram, "modelMatrix", objModelMatrix);
		SetVectorUniform(shaderProgram, "lightPos", lightPosition);
		SetVectorUniform(shaderProgram, "lightColor", lightColor);

		// 첫 번째 조명 정보 설정
		lights[0].lightPos = lightPosition;
		lights[0].padding = 0.0f; // 패딩은 사용하지 않지만 정렬을 위해 필요
		lights[0].lightColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간 조명

		// 두 번째 조명 정보 설정
<<<<<<< HEAD
		lights[1].lightPos = lightPosition + vec3(-1.0f, 0.0f, 0.0f);
=======
		lights[1].lightPos = lightPosition + vec3(-1.0f, 0.0f, -1.0f);
>>>>>>> origin/bum
		lights[1].padding = 0.0f; // 패딩은 사용하지 않지만 정렬을 위해 필요
		
		lights[1].lightColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // 파란 조명

		GLint lightPosLoc = glGetUniformLocation(shaderProgram.ID, "lights[0].lightPos");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram.ID, "lights[0].lightColor");

		// 첫 번째 조명 데이터
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lights[0].lightPos));
		glUniform4fv(lightColorLoc, 1, glm::value_ptr(lights[0].lightColor));

		// 두 번째 조명 데이터
		lightPosLoc = glGetUniformLocation(shaderProgram.ID, "lights[1].lightPos");
		lightColorLoc = glGetUniformLocation(shaderProgram.ID, "lights[1].lightColor");
		
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lights[1].lightPos));
		glUniform4fv(lightColorLoc, 1, glm::value_ptr(lights[1].lightColor));
<<<<<<< HEAD
=======
	
		FogInfo fogInfo;
		fogInfo.maxDist = 3.0f;
		fogInfo.minDist = 1.0f;
		fogInfo.color = vec3(1,1,1);
		fogInfo.padding = vec3(0, 0, 0);

		GLuint fogMaxDist = glGetUniformLocation(shaderProgram.ID,"Fog.maxDist");
		GLuint fogMinDist = glGetUniformLocation(shaderProgram.ID,"Fog.minDist");
		GLuint fogPaddingDist = glGetUniformLocation(shaderProgram.ID,"Fog.padding");
		GLuint fogColorDist = glGetUniformLocation(shaderProgram.ID,"Fog.color");
		glUniform1f(fogMaxDist, fogInfo.maxDist);
		glUniform1f(fogMinDist, fogInfo.minDist);
		glUniform3fv(fogPaddingDist, 1, glm::value_ptr(fogInfo.padding));
		glUniform3fv(fogColorDist, 1, glm::value_ptr(fogInfo.color));
>>>>>>> origin/bum
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
		//glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// First : Do Activate Shader AND SETTIGN VALUES

		//camera 
		camera.Inputs(window);
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
		 
		//Floor  
<<<<<<< HEAD
		floor.Draw(shaderProgram, camera);
		floor.Translate(shaderProgram, floorBasePos, vec3(0,0,0));

		//spere
		sphere.Draw(shaderProgram, camera);
		sphere.Translate(shaderProgram, floorBasePos, vec3(0,1,0));
=======
		floor.Translate(shaderProgram, floorBasePos, vec3(0,0,0));
		floor.Draw(shaderProgram, camera);
>>>>>>> origin/bum

		//spere
		for (int i = 0; i < 4; i++)
		{
			sphere.Translate(shaderProgram, floorBasePos, vec3(0, 0.2, 0.4 + i * -0.4));
			sphere.Draw(shaderProgram, camera);
		}
		// Light

		light.Translate(lightShader, lightPosition, vec3(0.0f, 0.0f, 0.0f)); 
		light.Draw(lightShader, camera); 
<<<<<<< HEAD
		light.Translate(lightShader, lightPosition, vec3(-1.0f, 0.0f, 0.0f)); 
=======
		light.Translate(lightShader, lightPosition, vec3(-1.0f, 0.0f, -1.0f)); 
>>>>>>> origin/bum
		light.Draw(lightShader, camera);

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