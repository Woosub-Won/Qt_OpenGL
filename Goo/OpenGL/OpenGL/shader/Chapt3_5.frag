#version 400

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

struct SpotLightInfo{
	vec4 position;
	vec3 intensity;
	vec3 direction;

	// ���� ���踦 ����� �� ���Ǵ� ����
	// �����ڸ��� ������ ��ο���
	float exponent;
	// �߽���� �����ڸ� ������ ����
	float cutoff;
};

struct MaterialInfo{
	vec3 Kd;
	vec3 Ka;
	vec3 Ks;
	float Shininess;
};


uniform SpotLightInfo Spot;
uniform MaterialInfo material;

vec3 adsWithSpotlight(){
	vec3 s = normalize(vec3(Spot.position)- Position);
	float angle = acos(dot(-s, Spot.direction));
	float cutoff = radians(clamp(Spot.cutoff, 0.0f, 90.0f));
	vec3 ambient = Spot.intensity*material.Ka;

	if(angle < cutoff){
		float spotFactor = pow(dot(-s, Spot.direction), Spot.exponent);
		vec3 v = normalize(vec3(-Position));
		vec3 h = normalize(v+s);
		vec3 diff = material.Kd*max(dot(s, Normal), 0.0f);
		vec3 spec = material.Ks*pow(max(dot(h, Normal), 0.0f), material.Shininess);
		return ambient+spotFactor*Spot.intensity*(diff+spec);
	}
	else{
		return ambient;
	}
}

void main(){
	FragColor = vec4(adsWithSpotlight(), 1.0f);
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

