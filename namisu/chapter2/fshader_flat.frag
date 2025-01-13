#version 400

flat in vec3 LightIntensity; // 보간되지 않은 조명 색상
layout(location = 0) out vec4 FragColor; // 출력 색상

void main() {
    FragColor = vec4(LightIntensity, 1.0); // 조명 색상을 출력
}
