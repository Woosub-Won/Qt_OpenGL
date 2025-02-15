#pragma once

#include "Mesh.h"

#include"OpenGL.h"

class MySphere : Mesh
{
public:
	MySphere();
	~MySphere();

	int NumVertices;
	 
	std::vector <Vertex> Vertices;
	void Init(int la_slice, int lo_slice, float radius = 0.1f, vec3 color = vec3(0.5, 0.5, 0.5)); 
	bool bInitialized;

	void Draw(Shader& shader, Camera& camera);

	void Translate(Shader& shader, vec3 basePosition, vec3 Translate) override;
};