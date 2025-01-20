#include "Cubemap.h"
#include "OpenGL.h"
#include "Camera.h"

#include <stb/stb_image.h>

Cubemap::Cubemap()
{ 
}

void Cubemap::Init()
{
	float skyboxVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,   
		 1.0f, -1.0f,  1.0f,   
		 1.0f, -1.0f, -1.0f,   
		-1.0f, -1.0f, -1.0f,   
		-1.0f,  1.0f,  1.0f,   
		 1.0f,  1.0f,  1.0f,   
		 1.0f,  1.0f, -1.0f,   
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

	unsigned int cubemapVBO, cubemapEBO;

	glGenVertexArrays(1, &cubemapVAO);
	glGenBuffers(1, &cubemapVBO);
	glGenBuffers(1, &cubemapEBO);

	glBindVertexArray(cubemapVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubemapEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	//초기화
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cubemap::BindTexture(std::string path)
{
	std::string facesCubemap[6] =
	{
		path + "/right.jpg",
		path + "/left.jpg",
		path + "/top.jpg",
		path + "/bottom.jpg",
		path + "/front.jpg",
		path + "/back.jpg"
	};

	// Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
}

void Cubemap::Draw(unsigned int slot)
{
	// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
	glDepthFunc(GL_LEQUAL); 
	// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
	// where an object is present (a depth of 1.0f will always fail against any object's depth value)
	glBindVertexArray(cubemapVAO);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);
}

void Cubemap::UpdateMatrix(Camera camera, unsigned int shaderID, int screenWidth, int screenHeight)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	//translate 삭제
	view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
	projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
}
