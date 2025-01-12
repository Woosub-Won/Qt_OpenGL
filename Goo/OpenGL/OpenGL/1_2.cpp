#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>

using namespace std;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(600, 800, "1-1", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
	}
	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mv = view * model;
		glm::vec4 transformed = mv * position;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}