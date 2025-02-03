#version 400

in vec3 curPos;
in vec3 normal;
in vec4 shadowCoords;

layout (location = 0) out vec4 FragColor;
 
uniform sampler2DShadow ShadowMap;
 

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass; 

subroutine (RenderPassType)
void recordDepth()
{
	// Do nothing, depth will be written automatically
}

void main() {
	// This will call either shadeWithShadow or recordDepth
	RenderPass();
}