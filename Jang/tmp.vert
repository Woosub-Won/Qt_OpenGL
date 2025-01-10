#version 400

in vec3 posAtt;
in vec3 colAtt;
out vec3 Color;

void main()
{
    Color = colAtt;
    gl_Position = vec4( posAtt, 1.0 );
}
