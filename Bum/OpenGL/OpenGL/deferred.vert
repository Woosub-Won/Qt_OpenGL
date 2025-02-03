#version 400 core
  
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec3 curPos;
out vec3 normal;
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

uniform mat4 shadowMatrix;

void main()
{   
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    texCoord = inTexCoords; 
}