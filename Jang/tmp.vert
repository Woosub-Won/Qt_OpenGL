#version 400

layout (location = 0) in vec3 posAtt;
layout (location = 1) in vec3 colAtt;
layout (location = 0) out vec3 Color;

void main()
{
    Color = colAtt;
    gl_Position = vec4( posAtt, 1.0 );
}
