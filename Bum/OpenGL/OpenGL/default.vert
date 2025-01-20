#version 400 core
 
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
 
out vec3 curPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord; 
out vec3 MapReflectDir;
out vec3 MapRefractDir;

out vec4 ProjTexCoord;

 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 
uniform mat4 projectorMatrix;

uniform vec3 camPos;

void main()
{   
	gl_Position = model * vec4(aPos, 1.0f);

	vec3 worldView = gl_Position.xyz - camPos;
	MapReflectDir = reflect(worldView, aNormal); 
	MapRefractDir = refract(worldView, aNormal, 0.3);

	gl_Position = projection * view * gl_Position;
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	
	curPos = gl_Position.xyz;
	Normal = normalMatrix * aNormal;
	color = aColor;
	texCoord = aTex; 

	ProjTexCoord = projectorMatrix * model * vec4(aPos,1.0f);
}