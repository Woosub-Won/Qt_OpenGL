#version 400 core

in vec2 texCoords;
out vec4 FragColor; 

uniform sampler2D screenTexture;  
uniform sampler2D baseTexture;

uniform float width;
uniform float height;

uniform float bloomStrength;

uniform float threshold;

vec4 gamma(vec4 color)
{
    return sqrt(color);
}
void main()
{    
    // 텍스처에서 색상 가져오기
     vec4 screenColor = texture(baseTexture, texCoords);   
     vec4 bloomColor = textureLod(screenTexture, texCoords, 5);
   
    vec3 highPassColor = (any(greaterThan(bloomColor.rgb, vec3(threshold)))) ? bloomColor.rgb : vec3(0.0);

    // Bloom 효과 
    vec4 bloom = vec4(highPassColor, 1.0) * bloomStrength;

    // 원본 색상과 Bloom 효과 결합
    //FragColor = gamma(screenColor +bloom );
    FragColor = (screenColor + bloom );
    
    //FragColor = bloomColor; 
}
