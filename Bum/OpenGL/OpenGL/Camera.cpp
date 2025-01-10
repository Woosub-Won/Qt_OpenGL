#include "Camera.h"

Camera::Camera(int width, int height, vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
} 
void Camera::UpdateMatrix(float FOVdegree, float nearPlane, float farPlane)
{
	mat4 view = mat4(1.0f);
	mat4 proj = mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	proj = glm::perspective(glm::radians(FOVdegree), (float)(width) / (height), nearPlane, farPlane);

	CameraMatrix = (proj * view);
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(
		glGetUniformLocation(shader.ID, uniform),
		1, GL_FALSE, glm::value_ptr(CameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += Curspeed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += Curspeed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += Curspeed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += Curspeed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		Position += Curspeed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		Position += Curspeed * -Up;
	} 
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		Curspeed = BaseSpeed * 1.5f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		Curspeed = BaseSpeed;
	}

	//mouse interaction
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//normalize cursor coord
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}
		double mouseX;
		double mouseY;

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotY = sensitivity * (float)(mouseX - (width / 2.0f)) / width;
		float rotX = sensitivity * (float)(mouseY - (height / 2.0f)) / height;

		vec3 newOrientation = rotate(Orientation, radians(-rotX), normalize(glm::cross(Orientation, Up)));
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		Orientation = rotate(Orientation, radians(-rotY), normalize(Up));

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;

	}

}
