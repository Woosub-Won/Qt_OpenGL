#version 400

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 LightIntensity;

struct LightInfo{
	vec4 Position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

uniform LightInfo Light;

struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};

uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectMatrix;
uniform mat4 MVP;

void getEyeSpace(out vec3 norm, out vec4 position){
	norm=normalize(NormalMatrix * aNormal);
	position = ModelViewMatrix * vec4(aPosition, 1.0f);
}

vec3 phongModel(vec4 position, vec3 norm){
	vec3 s = normalize(vec3(Light.Position - position));
	vec3 v = normalize(-position.xyz);
	vec3 r = 2*norm*(norm, s)-s;
	
	vec3 ambient = Light.La * Material.Ka;

	float sDotN = max(dot(s, norm), 0.0f);
	vec3 diffuse = Light.Ld * Material.Kd * sDotN;
	vec3 spec = vec3(0.0f);
	if(sDotN > 0.0f){
		spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0f), Material.Shininess);
	}
	return ambient + diffuse + spec;
}

void main(){
	vec3 eyeNorm;
	vec4 eyePosition;
	getEyeSpace(eyeNorm, eyePosition);
	LightIntensity = phongModel(eyePosition, eyeNorm);
	gl_Position = MVP * vec4(aPosition, 1.0f);
}