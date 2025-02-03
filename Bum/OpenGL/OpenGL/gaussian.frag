#version 400 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

uniform float width;
uniform float height;

subroutine vec4 RenderPassType();

subroutine uniform RenderPassType RenderPass;

// 17x17 커널을 위한 오프셋과 가중치
float pixelOffset[17] = float[](0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0);
float weight[17] = float[](
    0.19648, 0.17611, 0.1201, 0.0642, 0.0281,
    0.0109, 0.0039, 0.0013, 0.0004, 0.0001,
    0.00003, 0.00001, 0.000003, 0.000001, 0.0000003,
    0.0000001, 0.00000003
);

subroutine(RenderPassType)
vec4 Horizontal()
{   
    //vec4 bloomColor = textureLod(screenTexture, texCoords, 3);
    float dx = 1.0 / float(width);  // x 방향으로 변경
    vec4 sum = texture(screenTexture, texCoords) * weight[0];
    for (int i = 1; i < 17; i++) // 17개의 샘플링
    {
        sum += texture(screenTexture, texCoords + vec2(pixelOffset[i] * dx, 0)) * weight[i];
        sum += texture(screenTexture, texCoords - vec2(pixelOffset[i] * dx, 0)) * weight[i];
    }
    //sum = vec4(1,0,0,1);
    return sum;
}

subroutine(RenderPassType)
vec4 Vertical()
{
    float dy = 1.0 / float(height);  // y 방향으로 변경
    vec4 sum = texture(screenTexture, texCoords) * weight[0];
    for (int i = 1; i < 17; i++) // 17개의 샘플링
    {
        sum += texture(screenTexture, texCoords + vec2(0, pixelOffset[i] * dy)) * weight[i];
        sum += texture(screenTexture, texCoords - vec2(0, pixelOffset[i] * dy)) * weight[i];
    }
    //sum = vec4(0,1,0,1);
    return sum;
}

void main()
{
    FragColor = RenderPass();
}
