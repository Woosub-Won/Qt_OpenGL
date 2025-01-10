#pragma once

#include "OpenGL.h"
#include "vector"
#include "Texture.h"

class Pyramid
{
public:
	Pyramid();
	 
	void Init();
	void Draw();

	Shader GetShader() { return shaderProgram; }
protected:
	Shader shaderProgram;

	VAO vao;
	VBO vbo;
	EBO ebo;

	Texture texture; 

	std::vector<GLuint> indices;

};