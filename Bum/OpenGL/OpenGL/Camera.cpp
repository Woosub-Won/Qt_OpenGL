#include "Camera.h"

Camera::Camera(int width, int height, vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
	lastX = width / 2.0f;
	lastY = height / 2.0f;
	firstClick = true;
	sensitivity = 0.1f;  // 감도를 낮춰서 더 부드럽게 조정
	Orientation = vec3(0.0f, -0.5f, -1.0f);
	Up = vec3(0.0f, 1.0f, 0.0f);
	FOVdegree = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;
}

void Camera::UpdateMatrix()
{ 
	view = glm::lookAt(Position, Position + Orientation, Up);
	proj = glm::perspective(glm::radians(FOVdegree), (float)(width) / height, nearPlane, farPlane);
	
	CameraMatrix = proj * view;

	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, glm::value_ptr(view));
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(proj));

}

void Camera::Matrix(Shader& shader)
{
	/*
	glUniformMatrix4fv(
		glGetUniformLocation(shader.ID, uniform),
		1, GL_FALSE, glm::value_ptr(CameraMatrix));
		*/

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(proj));
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

	// Mouse interaction
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// 마우스 커서 숨기기
	   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// 현재 마우스 위치 가져오기
		double currentX, currentY;
		glfwGetCursorPos(window, &currentX, &currentY);

		// 첫 클릭시 초기화
		if (firstClick)
		{
			lastX = currentX;
			lastY = currentY;
			firstClick = false;
		}

		// 이동량 계산
		float xoffset = (currentX - lastX);
		float yoffset = (lastY - currentY);  // 반전된 y축

		// 이전 위치 업데이트
		lastX = currentX;
		lastY = currentY;

		// 너무 큰 변화량 제한
		const float maxOffset = 100.0f;
		xoffset = glm::clamp(xoffset, -maxOffset, maxOffset);
		yoffset = glm::clamp(yoffset, -maxOffset, maxOffset);

		// 감도 적용
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		// 수직 회전 (피치)
		vec3 newOrientation = glm::rotate(Orientation, glm::radians(yoffset),
			glm::normalize(glm::cross(Orientation, Up)));

		// 수직 회전 제한 (80도)
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(80.0f))
		{
			Orientation = newOrientation;
		}

		// 수평 회전 (요)
		Orientation = glm::rotate(Orientation, glm::radians(-xoffset), Up);

		// 방향 벡터 정규화
		Orientation = glm::normalize(Orientation);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}