#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader {
public:
	GLuint ID;
	Shader(const char*,const char*);
	void use();
private:
	void readShader(const char*, string&);
	void checkCompileErrors(GLuint, string);
};


#endif SHADER_H