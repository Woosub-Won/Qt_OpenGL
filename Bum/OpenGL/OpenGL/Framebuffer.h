#pragma once
class Framebuffer
{
public:
	Framebuffer() {};
	Framebuffer(int screenWidth, int screenHeight);

	void Init(int screenWidth, int screenHeight);
	 
	void GenFrameBuffer();
	void BindTextureBuffer();
	void BindRenderBuffer();
	void Draw();
	  
	unsigned int FBO;
protected:
	unsigned int rectVAO;
	unsigned int framebufferTexture;
	int width;
	int height;

	bool bInit = false; 
};

