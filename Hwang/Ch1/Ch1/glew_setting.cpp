#define GLM_ENABLE_EXPERIMENTAL
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
using glm::vec3;
using glm::mat4;

int main() {

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
	}

	if (!GLEW_ARB_vertex_program) {
		fprintf(stderr, "ARB_vertex_program is missing!\n");
	}

	glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 5.0),
								glm::vec3(0.0, 0.0, 0.0),
								glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0));
	glm::mat4 mv = view * model;
	glm::vec4 transformed = mv * position;
	
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor    : %s\n", vendor);
	printf("GL Renderer  : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	if (0 == vertShader) {
		fprintf(stderr, "Error creating vertex shader.\n");
		exit(1);
	}

	const GLchar* shaderCode = loadShaderAsString("basic.vert");
	const GLchar* codeArray[] = { shaderCode };
	glShaderSource(vertShader, 1, codeArray, NULL);

	glCompileShader(vertShader);

	GLint result;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		fprintf(stderr, "Vertex shader compilation failed!\n");
		GLint logLen;
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(vertShader, logLen, &written, log);
			fprintf(stderr, "Shader log:\n%s", log);
			free(log);
		}
	}
}