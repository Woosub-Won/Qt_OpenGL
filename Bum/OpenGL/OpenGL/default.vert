#version 400 core
 
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 curPos;
out vec3 normal;
out vec3 color;
out vec2 texCord; 
 
uniform mat4 modelMatrix;
uniform mat4 camMatrix;

void main()
{ 
	curPos = vec3(modelMatrix * vec4(aPos, 1.0f));
	gl_Position = camMatrix * vec4(curPos,1.0f);

	color = aColor;
	texCord = aTex;
	normal = aNormal;
}