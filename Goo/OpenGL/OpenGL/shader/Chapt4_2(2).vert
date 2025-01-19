#version 400

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main(){
	TexCoord = aTexCoord;
	Normal = normalize(NormalMatrix*aNormal);
	Position = vec3(ModelViewMatrix*vec4(aPosition, 1.0f));

	gl_Position = MVP*vec4(aPosition, 1.0f);
}