#ifndef CAMERA_H
#define CAMERA_H
#include "OpenGL.h"

class Camera
{
public: 
	Camera(int width, int hieght, vec3 Position);

	vec3 Position;
	
	void UpdateMatrix();
	void Matrix(Shader& shader);
	void Inputs(GLFWwindow* window);
	
	vec3 Up = vec3(0.0f, 1.0f, 0.0f);
	vec3 Orientation = vec3(0.0f, -1.5f, -1.0f);
	 
	mat4 GetView() { return view; }
	float GetFOV() { return FOVdegree; }
	float GetWidth() { return width; }
	float GetHeight() { return height; }
	float GetNearPlane() { return nearPlane; }
	float GetFarPlane() { return farPlane; }

protected:   
	mat4 view = mat4(1.0f);
	mat4 proj = mat4(1.0f);

	float FOVdegree;

	float nearPlane = 0.1f;
	float farPlane = 100.0f;

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
