#version 400 core
 

in vec2 texCoords;

 
out vec4 FragColor;

uniform sampler2D screenTexture; 

uniform float width;
uniform float height;

 
float EdgeThreshold = 0.1;

float luma( vec3 color ) {
 return 0.2126 * color.r + 0.7152 * color.g + 
 0.0722 * color.b;
}

void main()
{
	//FragColor = vec4(1,0,0,0);


	float dx = 1.0 / float(width);
	float dy = 1.0 / float(height);
	
	float s00 = luma(texture( screenTexture, texCoords + vec2(-dx,dy) ).rgb); 
	float s10 = luma(texture( screenTexture, texCoords + vec2(-dx,0.0) ).rgb);
	float s20 = luma(texture( screenTexture, texCoords + vec2(-dx,-dy) ).rgb);
	float s01 = luma(texture( screenTexture, texCoords + vec2(0.0,dy) ).rgb);
	float s21 = luma(texture( screenTexture, texCoords + vec2(0.0,-dy) ).rgb);
	float s02 = luma(texture( screenTexture, texCoords + vec2(dx, dy) ).rgb);
	float s12 = luma(texture( screenTexture, texCoords + vec2(dx, 0.0) ).rgb);
	float s22 = luma(texture( screenTexture, texCoords + vec2(dx, -dy) ).rgb);
	
	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
	float dist = sx * sx + sy * sy;
	
	if( dist > EdgeThreshold )
		FragColor = vec4(1.0);
	else
		FragColor = vec4(0.0,0.0,0.0,1.0);
	
		 
}