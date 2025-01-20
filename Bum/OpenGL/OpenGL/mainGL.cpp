#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"


#include <iostream> 
#include <stb/stb_image.h>

#include "OpenGL.h"

#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Mesh.h"
#include "MySphere.h"
#include "Light.h"
#include "Framebuffer.h"
#include "FogFactor.h"
#include "Cubemap.h"

const float screenHeight = 800;
const float screenWidth = screenHeight * 1.6f;
const vec4 baseColor = vec4(0.07f, 0.13f, 0.17f, 1.0f);

using namespace std;

// Vertices coordinates
// 1  2 
// 0  3
 
float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


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
void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "OpenGL Debug Message: " << message << std::endl;
}
int main()
{
	// openGL init
	glfwInit();

	// verion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

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
		Texture("planks.png", "tex", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("frog.png", "tex", 1, GL_RGBA, GL_UNSIGNED_BYTE)
	};

	//Shader
	Shader shaderProgram("default.vert", "default.frag");
	Shader outliningShader("outlining.vert", "outlining.frag");
	Shader lightShader("light.vert", "light.frag");
	Shader framebufferShader("framebuffer.vert", "framebuffer.frag");
	Shader cubemapShader("skybox.vert", "skybox.frag");
	Shader normalShader("normal.vert", "normal.frag", "normal.geom");

	// Prepare framebuffer rectangle VBO and VAO
	Framebuffer frameBuffer;
	frameBuffer.Init(screenWidth, screenHeight);

	//skybox
	Cubemap cubemap;
	cubemap.Init();

	std::string cubemapPath = "skybox";
	cubemap.BindTexture(cubemapPath);

	//Floor 
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
	sphere.Init(30, 30,tex);

	// light
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	// Create light mesh
	Mesh light(lightVerts, lightInd, tex);

	//Camera
	Camera camera(screenWidth, screenHeight, vec3(0.0f, 1.0f, 2.0f));

	// Uniform  
	vec3 lightPosition = vec3(0.5f, 1.5f, 0.5f);
	vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//shaderProgram 
	LightInfo lights[2];
	{
		shaderProgram.Activate();

		mat4 objModelMatrix = mat4(1.0f);
		objModelMatrix = translate(objModelMatrix, vec3(0.0f, 0.0f, 0.0f)) * scale(objModelMatrix, vec3(2.0f, 2.0f, 2.0f));

		SetMatrixUniform(shaderProgram, "model", objModelMatrix);

		//SetVectorUniform(shaderProgram, "lightPos", lightPosition); 
		//SetVectorUniform(shaderProgram, "lightColor", lightColor);

		// 첫 번째 조명 정보 설정
		lights[0].lightPos = lightPosition;
		lights[0].padding = 0.0f; // 패딩은 사용하지 않지만 정렬을 위해 필요
		lights[0].lightColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간 조명

		// 두 번째 조명 정보 설정
		lights[1].lightPos = lightPosition + vec3(-1.0f, 0.0f, -1.0f);
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

		FogInfo fogInfo;
		fogInfo.maxDist = 3.0f;
		fogInfo.minDist = 1.0f;
		fogInfo.color = vec3(baseColor.x, baseColor.y, baseColor.z);
		fogInfo.padding = vec3(0, 0, 0);

		GLuint fogMaxDist = glGetUniformLocation(shaderProgram.ID, "Fog.maxDist");
		GLuint fogMinDist = glGetUniformLocation(shaderProgram.ID, "Fog.minDist");
		GLuint fogPaddingDist = glGetUniformLocation(shaderProgram.ID, "Fog.padding");
		GLuint fogColorDist = glGetUniformLocation(shaderProgram.ID, "Fog.color");
		glUniform1f(fogMaxDist, fogInfo.maxDist);
		glUniform1f(fogMinDist, fogInfo.minDist);
		glUniform3fv(fogPaddingDist, 1, glm::value_ptr(fogInfo.padding));
		glUniform3fv(fogColorDist, 1, glm::value_ptr(fogInfo.color));
	
 
		//projectorMatrix
		//0,1,2
		vec3 projPos = camera.Position + vec3(-0.2f, 0, 0.2f);
		// 0 -0.5, -1
		vec3 projAt = camera.Orientation;
		vec3 projUp = camera.Up;

		mat4 projView = glm::lookAt(projPos, projPos + projAt, projUp);
		mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.1f, 100.0f);
		//normalization
		mat4 projScaleTrans = glm::translate(vec3(0.5f)) *
			glm::scale(vec3(0.5f));
		mat4 m = projScaleTrans * projProj * projView;
		
		// Set the uniform variable
		SetMatrixUniform(shaderProgram, "projectorMatrix", m); 
	}

	// light shader
	{
		lightShader.Activate();
		mat4 lightModelMatrix = mat4(1.0f);
		lightModelMatrix = translate(lightModelMatrix, lightPosition);

		SetMatrixUniform(lightShader, "modelMatrix", lightModelMatrix);
		SetVectorUniform(lightShader, "lightColor", lightColor);
	}

	//framebuffer shader
	{
		framebufferShader.Activate();
		glUniform1i(glGetUniformLocation(framebufferShader.ID, "screenTexture"), 0);
		glUniform4fv(glGetUniformLocation(framebufferShader.ID, "baseColor"), 1, value_ptr(baseColor));
		glUniform1f(glGetUniformLocation(framebufferShader.ID, "width"), screenWidth);
		glUniform1f(glGetUniformLocation(framebufferShader.ID, "height"), screenHeight);
	}

	//cubemap
	{
		cubemapShader.Activate();
		glUniform1i(glGetUniformLocation(cubemapShader.ID, "skybox"), 10);
	}
	
	//normal
	{ 
	}

	//imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 400");

	bool bFogEffect = true;

	double prevTime = 0.0f;
	double curTime = 0.0f;
	double timeDiff;
	unsigned int counter = 0;

	glEnable(GL_DEPTH_TEST);

	//stencil
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  
	glEnable(GL_DEBUG_OUTPUT);   
	// Create Frame Buffer Object
	frameBuffer.GenFrameBuffer();

	// Create Framebuffer Texture
	frameBuffer.BindTextureBuffer();
	frameBuffer.BindRenderBuffer();


	while (!glfwWindowShouldClose(window))
	{
		//Init
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.FBO);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(baseColor.x, baseColor.y, baseColor.z, baseColor.w); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//Set FPS
		curTime = glfwGetTime();
		timeDiff =	curTime - prevTime;
		counter++;
		if (timeDiff > 1.0f / 60.0f)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "OpenGL - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = curTime;
			counter = 0; 
		}
		//camera 
		if (!io.WantCaptureMouse)
		{
			camera.Inputs(window);
			camera.UpdateMatrix(45.0f, 0.1f, 100.0f); 
		}
		
		//Floor  
		floor.Translate(shaderProgram, floorBasePos, vec3(0,0,0));
		floor.Draw(shaderProgram, camera);
		//glBindTexture(GL_TEXTURE_2D, 0);

		//spere
		for (int i = 0; i < 4; i++)
		{ 
			sphere.Translate(shaderProgram, floorBasePos, vec3(0, 0.2, 0.4 + i * -0.4));
			sphere.Draw(shaderProgram, camera); 
			//glLineWidth(2.0f); 
			//sphere.Draw(normalShader, camera);   
		}
		//glBindTexture(GL_TEXTURE_2D, 0);
		  
		// Light
		light.Translate(lightShader, lightPosition, vec3(0.0f, 0.0f, 0.0f)); 
		light.Draw(lightShader, camera); 
		light.Translate(lightShader, lightPosition, vec3(-1.0f, 0.0f, -1.0f)); 
		light.Draw(lightShader, camera);
		 
		// cubemap
		cubemapShader.Activate();
		cubemap.UpdateMatrix(camera, cubemapShader.ID, screenWidth, screenHeight);
		cubemap.Draw(10);

		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// Draw the framebuffer rectangle
		framebufferShader.Activate(); 
		frameBuffer.Draw();  

		//imgui init
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Opentions");
		ImGui::Text("Selection");

		ImGui::Checkbox("Fog Effect", &bFogEffect);
		glUseProgram(shaderProgram.ID);
		int uniformLocation = glGetUniformLocation(shaderProgram.ID, "bFogEffect");
		glUniform1i(uniformLocation, bFogEffect);

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	//vao1.Delete();
	//vbo1.Delete();
	//ebo1.Delete();
	//texture.Delete();
	//shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}