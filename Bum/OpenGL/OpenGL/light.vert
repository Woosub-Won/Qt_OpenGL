#version 400 core
 
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCord; 

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

void main()
{ 
	gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0f);
	//gl_Position =  vec4(aPos, 1.0f);

	color = aColor;
	texCord = aTex;
}