#pragma once
#include "string"

class Camera;

class Cubemap
{
public:
	Cubemap();
	
	void Init(); 
	void BindTexture(std::string path);
	void Draw(unsigned int slot);
	void Draw_Deferred(unsigned int slot, unsigned int ShaderID, unsigned int depthTex);
	void UpdateMatrix(Camera camera, unsigned int shaderID, int screenWidth, int screenHeight);

	unsigned int GetVAO() { return cubemapVAO; }
	unsigned int GetTexture() { return cubemapTexture; }
protected:

	unsigned int cubemapVAO;
	unsigned int cubemapTexture;
	
};

