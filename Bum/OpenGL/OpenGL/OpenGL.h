#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

#include"VAO.h"
#include"VBO.h"
#include"EBO.h" 

#include <type_traits> 

using glm::mat4;
using glm::mat3;
using glm::vec4;
using glm::vec3;
using glm::vec2;

using glm::translate;
using glm::rotate;
using glm::perspective;
using glm::normalize;
using glm::radians;

template<typename T>
static void SetMatrixUniform(Shader& shader, const char* uniform, T mat, bool transpose = false)
{ 
	if constexpr (std::is_same_v<T, glm::mat4>)
	{
		if (transpose)
		{
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform),
				1, GL_TRUE, glm::value_ptr(mat));
		}
		else
		{ 
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform),
				1, GL_FALSE, glm::value_ptr(mat));
		}
	}
	else if constexpr (std::is_same_v(T, glm::mat3))
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform),
			1, GL_FALSE, glm::value_ptr(mat));
	}
}

template<typename T>
static void SetVectorUniform(Shader& shader, const char* uniform, T vec, bool transpose = false)
{
	if constexpr (std::is_same_v<T, glm::vec4>)
	{ 
		glUniform4f(glGetUniformLocation(shader.ID, uniform),
			vec.x, vec.y, vec.z, vec.w); 
	}
	else if constexpr (std::is_same_v<T, glm::vec3>)
	{
		glUniform3f(glGetUniformLocation(shader.ID, uniform),
			vec.x, vec.y, vec.z);
	}
}