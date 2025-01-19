#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor; 
uniform sampler2D Tex1;
uniform sampler2D Tex2;

struct LightInfo{
	vec4 position;
	vec3 intensity;
};

struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float shininess;
};

uniform LightInfo light;
uniform MaterialInfo material;

void phongModel (vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec){
	vec3 n = normalize(norm);
	vec3 s = normalize(vec3(light.position)-pos);
	vec3 v = normalize(vec3(-pos));
	vec3 r = 2.0f*n*dot(s, n)-s;

	vec3 ambi = material.Ka;
	vec3 diff = material.Kd*max(dot(n, s), 0.0f);
	spec = material.Ks*pow(max(dot(r, v), 0.0f), material.shininess);
	ambAndDiff = light.intensity*(ambi+diff);
	
}

void main(){
	vec3 ambAndDiff, spec;
	vec4 texColor1 = texture(Tex1, TexCoord);
	vec4 texColor2 = texture(Tex2, TexCoord);
	vec4 finalTexColor = mix(texColor1, texColor2, 0.2f);
	phongModel(Position, Normal, ambAndDiff, spec);
	FragColor = vec4(ambAndDiff, 1.0f)*finalTexColor+vec4(spec, 1.0f);
}