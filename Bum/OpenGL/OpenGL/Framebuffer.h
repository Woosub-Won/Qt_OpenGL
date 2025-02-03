#pragma once
class Framebuffer
{
public:
	Framebuffer() {};
	Framebuffer(int screenWidth, int screenHeight);

	void Init(int screenWidth, int screenHeight);
	void Init_Deferred(int screenWidth, int screenHeight);
	void Init_Shadow(int screenWidth, int screenHeight);

	 
	void GenFrameBuffer();
	void BindBuffer(); 
	void Draw();
	void Draw(const unsigned int shaderID, const unsigned int* textures, const char* sameplers[], int textureCount);
	void Draw_Deferred(unsigned int ShaderID);
	void Draw_Shadow(unsigned int shaderID);

	unsigned int FBO;
	unsigned int RBO;
	unsigned int depthTex, posTex, normTex, colorTex; 
	unsigned int shadowDepthTex;;

	unsigned int framebufferTexture;

protected:
	unsigned int rectVAO;
	int width;
	int height;

	bool bInit = false; 
};

