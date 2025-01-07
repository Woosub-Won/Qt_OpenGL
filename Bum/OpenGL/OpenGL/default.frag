#version 330 core
 
in vec3 color;
in vec2 texCord;

out vec4 FragColor;

uniform sampler2D tex0;

void main()
{
	//FragColor = vec4(color, 1.0f);

	FragColor = texture(tex0, texCord);
}