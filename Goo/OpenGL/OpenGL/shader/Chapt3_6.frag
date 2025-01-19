#version 400

in vec3 Position;
in vec3 Normal;

struct LightInfo{
	vec4 position;
	vec3 intensity;
};

struct MaterialInfo{
	vec3 Kd;
	vec3 Ka;
};

uniform LightInfo light;
uniform MaterialInfo material;

const int levels = 3;
const float scaleFactor = 1.0f/levels;

layout (location = 0) out vec4 FragColor;

vec3 toonShade(){
	vec3 n = normalize(Normal);
	vec3 s = normalize(light.position.xyz - Position.xyz);
	float cosine = max(0.0f, dot(s, n));
	vec3 diffuse = material.Kd*floor(cosine*levels)*scaleFactor;

	return light.intensity*(material.Ka+diffuse);
}

void main(){
	FragColor = vec4(toonShade(), 1.0f);
}