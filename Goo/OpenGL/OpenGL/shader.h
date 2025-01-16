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
	void setBool(const string&, bool value) const;
	void setInt(const string&, int value) const;
	void setFloat(const string&, float) const;
	void setVec2(const string&, const glm::vec2&) const;
	void setVec3(const string&, const glm::vec3&) const;
	void setVec4(const string&, const glm::vec4&) const;
	void setVec2(const string&, float, float) const;
	void setVec3(const string&, float, float, float) const;
	void setVec4(const string&, float, float, float, float) const;
	void setMat2(const string&, const glm::mat2&) const;
	void setMat3(const string&, const glm::mat3&) const;
	void setMat4(const string&, const glm::mat4&) const;
private:
	void readShader(const char*, string&);
	void checkCompileErrors(GLuint, string);
};


#endif SHADER_H