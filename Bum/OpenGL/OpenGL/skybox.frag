#version 400 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform sampler2D DepthTex;  
uniform vec2 screenSize;
 
void main()
{ 
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    
    float depth = texture(DepthTex, screenUV).r;
    if (depth >= 1.0 - 1e-6) 
    {
        FragColor = texture(skybox, texCoords);  
    } 
    else
    {
        discard;
    }

} 