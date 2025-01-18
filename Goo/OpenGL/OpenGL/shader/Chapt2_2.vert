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
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main(){
	vec3 tnorm = normalize(NormalMatrix*aNormal);
	vec4 eyeCoords = ModelViewMatrix*vec4(aPosition, 1.0f);

	// 광원 위치도 ViewMatrix로 곱해준 뒤 셰이더로 전달함
	vec3 s = normalize(vec3(Light.Position-eyeCoords));
	// 뷰 공간에선 카메라가 원점이고, v 벡터는 (카메라 위치 - 정점 위치)이므로 아래와 같음 
	vec3 v = normalize(-eyeCoords.xyz);
	//vec3 r = reflect(-s, tnorm);
	vec3 r = 2*tnorm*dot(tnorm, s)-s;

	vec3 ambient = Light.La * Material.Ka;
	
	float diffuseIntensity = max(dot(s, tnorm), 0.0f);
	vec3 diffuse = Light.Ld*Material.Kd*diffuseIntensity;
	float specularIntensity = 0;
	if(diffuseIntensity > 0.0f){
		specularIntensity = pow(max(dot(r, v), 0.0f), Material.Shininess);
	}

	vec3 specular = Light.Ls*Material.Ks*specularIntensity;
	
	LightIntensity = ambient+diffuse+specular;
	gl_Position = MVP*vec4(aPosition, 1.0f);
}
	