#ifndef CAMERA_H
#define CAMERA_H
#include "OpenGL.h"

class Camera
{
public: 
	Camera(int width, int hieght, vec3 Position);

	vec3 Position;
	
	void UpdateMatrix(float FOVdegree, float nearPlane, float farPlane);
	void Matrix(Shader& shader);
	void Inputs(GLFWwindow* window);
	
	vec3 Up = vec3(0.0f, 1.0f, 0.0f);
	vec3 Orientation = vec3(0.0f, -1.5f, -1.0f);
	 
protected:   
	mat4 view = mat4(1.0f);
	mat4 proj = mat4(1.0f);

	int width;
	int height;
	float lastX;
	float lastY;

	bool firstClick = true;


	float BaseSpeed = 0.1f;
	float Curspeed = 0.01f;
	float sensitivity = 100.0f;

	mat4 CameraMatrix;
};

#endif
