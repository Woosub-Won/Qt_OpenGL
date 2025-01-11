#ifndef CAMERA_H
#define CAMERA_H
#include "OpenGL.h"

class Camera
{
public: 
	Camera(int width, int hieght, vec3 Position);

	vec3 Position;
	
	void UpdateMatrix(float FOVdegree, float nearPlane, float farPlane);
	void Matrix(Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
	 
protected:  
	vec3 Orientation = vec3(0.0f, -0.5f, -1.0f);
	vec3 Up = vec3(0.0f, 1.0f, 0.0f);

	int width;
	int height;
	bool firstClick = true;


	float BaseSpeed = 0.03f;
	float Curspeed = 0.01f;
	float sensitivity = 100.0f;

	mat4 CameraMatrix;
};

#endif
