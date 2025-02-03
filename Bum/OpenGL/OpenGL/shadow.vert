#version 400
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;

out vec3 curPos; 
out vec3 normal; 
out vec4 shadowCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 

uniform mat4 shadowMatrix;

void main()
{
	gl_Position = model * vec4(aPos, 1.0f);
	curPos = gl_Position.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	normal = normalize( normalMatrix * aNormal );

	// ShadowMatrix converts from modeling coordinates
	// to shadow map coordinates.

	shadowCoords = shadowMatrix* model * vec4(aPos,1.0);
	gl_Position = projection * view * gl_Position;
	
}