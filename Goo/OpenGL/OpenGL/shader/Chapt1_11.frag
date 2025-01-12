#version 400

in vec3 TexCoord;
layout (location = 0) out vec4 FragColor;

uniform BlobSettings{
	vec4 InnerColor;
	vec4 OuterColor;
	float RadiusInner;
	float RadiusOuter;
};

void main(){
	float dx = TexCoord.x - 0.5f;
	float dy = TexCoord.y - 0.5f;
	float dist = sqrt(dx*dx+dy*dy);
	FragColor = mix(InnerColor, OuterColor, smoothstep(RadiusInner, RadiusOuter, dist));
}