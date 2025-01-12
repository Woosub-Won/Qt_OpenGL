#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	const char* vertexCode=NULL;
	const char* fragmentCode=NULL;
	string vShaderCode;
	string fShaderCode;
	readShader(vertexPath, vShaderCode);
	readShader(fragmentPath, fShaderCode);

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	if (0 == vertShader) {
		fprintf(stderr, "Error createing vertex shader.\n");
		exit(1);
	}
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (0 == vertShader) {
		fprintf(stderr, "Error createing fragment shader.\n");
		exit(1);
	}
	vertexCode = vShaderCode.c_str();
	fragmentCode = fShaderCode.c_str();
	glShaderSource(vertShader, 1, &vertexCode, NULL);
	glShaderSource(fragShader, 1, &fragmentCode, NULL);

	glCompileShader(vertShader);
	glCompileShader(fragShader);

	checkCompileErrors(vertShader, "VERTEX");
	checkCompileErrors(fragShader, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertShader);
	glAttachShader(ID, fragShader);
	glLinkProgram(ID);
	
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

}

void Shader::readShader(const char* path, string& code){
	ifstream shaderFile;
	shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	
	try {
		shaderFile.open(path);
		stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		code = shaderStream.str();
	}
	catch (ifstream::failure& e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ : " << e.what() << "\n";
	}

}

void Shader::checkCompileErrors(GLuint shader, string type) {
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type : " << type << "\n" << infoLog << "\n";
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::PROGRAM_LINKING_ERROR of type : " << type << "\n" << infoLog << "\n";
		}
	}
}

void Shader::use() {
	glUseProgram(ID);
}