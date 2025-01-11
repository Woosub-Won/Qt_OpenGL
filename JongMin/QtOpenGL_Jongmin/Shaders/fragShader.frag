// #version 400
// in vec3 Color;
// out vec4 FragColor;
// void main() {
//     FragColor = vec4(Color, 1.0);
// }

#version 400
in vec3 TexCoord; // Vertex Shader에서 전달받은 텍스처 좌표
layout (location = 0) out vec4 FragColor; // 출력 색상

uniform BlobSettings {
    vec4 InnerColor;    // 원 내부 색상
    vec4 OuterColor;    // 원 외부 색상
    float RadiusInner;  // 내부 고정 색상 반지름
    float RadiusOuter;  // 외부 부드러운 경계 반지름
};

void main() {
    // 텍스처 좌표를 기준으로 중심(0.5, 0.5)에서의 거리 계산
    float dx = TexCoord.x - 0.5;
    float dy = TexCoord.y - 0.5;
    float dist = sqrt(dx * dx + dy * dy); // 거리 계산
    //FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
    // 부드러운 경계 생성: RadiusInner와 RadiusOuter 사이에서 색상 혼합
    //FragColor = BlobSettings.InnerColor;
    //float dx = RadiusInner;
    FragColor = mix(InnerColor, OuterColor, smoothstep(RadiusInner, RadiusOuter, dist)); //This Fucking Qt works in red line. shit.

}
