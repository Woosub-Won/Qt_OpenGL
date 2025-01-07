#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{ 
	type = texType;
	
	int t_width, t_height, numColCh;
	unsigned char* bytes = stbi_load(image, &t_width, &t_height, &numColCh, STBI_rgb_alpha);

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(type, ID);

	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//float flatColor[] = { 0,0,0,1.0f};
	//glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, flatColor);

	//glTexImage2D(GL_TEXTURE0, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glTexImage2D(type, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	glGenerateMipmap(type);

	stbi_image_free(bytes);
	glBindTexture(type, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{  
	GLuint UniTex0 = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(UniTex0, unit); 
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}