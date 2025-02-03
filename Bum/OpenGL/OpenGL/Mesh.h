#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;
	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the mesh
	Mesh() {};
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

	virtual void Init(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

	// Draws the mesh
	virtual void Draw(Shader& shader, Camera& camera);
	virtual void Draw_Shadow(Shader& shader, Camera& camera, vec3 lightPos);

	virtual void Translate(Shader& shader, vec3 basePosition, vec3 Translate);

	void UpdateVPMatrix(Shader& shader, Camera& camera);
};

#endif