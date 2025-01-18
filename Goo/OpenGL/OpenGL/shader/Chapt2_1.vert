#version 400

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

out vec3 LightIntensity;

uniform vec4 LightPosition;
uniform vec3 Kd; // material 계수
uniform vec3 Ld; // 광원 계수

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main(){
	vec3 tnorm = normalize(NormalMatrix*Normal);
	// eyeCoords : 카메라의 위치 X , 뷰 행렬까지 곱한 값 O
	vec4 eyeCoords = ModelViewMatrix*vec4(Position, 1.0f);
	vec3 s = normalize(vec3(LightPosition - eyeCoords));

	LightIntensity = Ld*Kd*max(dot(s, tnorm), 0.0f);
	gl_Position = MVP*vec4(Position, 1.0f);
}