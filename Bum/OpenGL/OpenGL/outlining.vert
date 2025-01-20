#version 400 core
 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 curPos; 

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

uniform float outlining;

void main()
{ 
	curPos = vec3(modelMatrix * vec4(aPos + aNormal * outlining, 1.0f));
	gl_Position = camMatrix * vec4(curPos,1.0f);
}