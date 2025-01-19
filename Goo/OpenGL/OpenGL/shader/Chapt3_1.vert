#version 400

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 Color;

struct LightInfo{
	vec4 Position;
	vec3 Intensity;
};

uniform LightInfo lights[5];

struct MaterialInfo{
	vec3 Kd;
	vec3 Ka;
	vec3 Ks;
	float Shininess;
};

uniform MaterialInfo material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

vec3 ads(int lightIndex, vec4 position, vec3 norm){
	vec3 s = normalize(vec3(lights[lightIndex].Position-position));
	vec3 v = normalize(vec3(-position));
	vec3 r = 2*norm*dot(norm, s)-s;
	vec3 I = lights[lightIndex].Intensity;
	
	vec3 ambi=material.Ka;
	vec3 diff=material.Kd*max(dot(s, norm), 0.0f);
	vec3 spec=material.Ks*pow(max(dot(r, v), 0.0f),material.Shininess);
	vec3 result = I*( ambi + diff + spec);
	return result;
}

void main(){
	vec3 eyeNorm = normalize(NormalMatrix*aNormal);
	vec4 eyePosition = ModelViewMatrix*vec4(aPosition, 1.0f);

	Color = vec3(0.0f);
	for(int i=0; i < 5; i++){
		Color += ads(i, eyePosition, eyeNorm);
	}
	gl_Position = MVP*vec4(aPosition, 1.0f);
}