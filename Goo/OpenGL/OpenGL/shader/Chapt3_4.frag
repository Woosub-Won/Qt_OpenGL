#version 400

in vec3 Position;
in vec3 Normal;

layout(location = 0) out vec4 FragColor;

struct LightInfo{
	vec4 Position;
	vec3 Intensity;
};

struct MaterialInfo{
	vec3 Kd;
	vec3 Ka;
	vec3 Ks;
	float Shininess;
};

uniform LightInfo light;
uniform MaterialInfo material;

vec3 ads(){
	vec3 n = normalize(Normal);
	vec3 s = normalize(vec3(light.Position)-Position);
	vec3 v = normalize(vec3(-Position));
	vec3 h = normalize(v+s);

	vec3 ambi = material.Ka;
	vec3 diff = material.Kd*max(dot(n, s), 0.0f);
	vec3 spec = material.Ks*pow(max(dot(h, n), 0.0f), material.Shininess);
	vec3 result = light.Intensity*(ambi+diff+spec);

	return result;
}

void main(){
	FragColor = vec4(ads(), 1.0f);
}