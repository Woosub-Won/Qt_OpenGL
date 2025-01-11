#version 400 core
 
in vec3 color;
in vec2 texCord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec4 lightColor;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) * lightColor; 
	//FragColor = texture(tex0, texCord);
}