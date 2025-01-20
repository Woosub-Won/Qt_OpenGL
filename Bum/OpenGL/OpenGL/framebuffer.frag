#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform samplerCube skybox;
uniform vec4 baseColor;

uniform float width;
uniform float height;

vec2 offset[9] = vec2[9](
    vec2(-1.0 / width,  1.0 / height), vec2(0.0,  1.0 / height), vec2( 1.0 / width,  1.0 / height),
    vec2(-1.0 / width,  0.0),          vec2(0.0,  0.0),          vec2( 1.0 / width,  0.0),
    vec2(-1.0 / width, -1.0 / height), vec2(0.0, -1.0 / height), vec2( 1.0 / width, -1.0 / height)
);

    
float kernel[9] = float[9]
(
    2,2,2,
    2,-20,2,
    2,2,2
);

vec4 KernelFilter()
{
    vec4 color= vec4(0,0,0,1);
    for(int i = 0; i < 9;i ++)
    { 
         color += texture(screenTexture, vec2(texCoords + offset[i])) * kernel[i];
    }
    return color;
}
bool vec4Equal(vec4 value0, vec4 value1)
{
    float epsilon = 0.01; 
    bool isCloseEnough = true;
    isCloseEnough = isCloseEnough && abs(value0.r - value1.r) < epsilon;
    isCloseEnough = isCloseEnough && abs(value0.g - value1.g) < epsilon;
    isCloseEnough = isCloseEnough && abs(value0.b - value1.b) < epsilon;
    
    return isCloseEnough;
}
void main()
{
   //È¸»ö
   vec4 color = texture(screenTexture, texCoords);
   //float avg = (color.r + color.g + color.b) / 3.0f;
   //FragColor = vec4(avg);
     
    //if(vec4Equal(color, baseColor)) {
    //    color = vec4(1, 0, 0, 0);
    //}
    //else 
    //{
    //   color = KernelFilter(); 
    //}
    FragColor = color; 
}