#version 400
in vec4 vPosition; // 위치
in vec4 vNormal;   // 법선
in vec4 sNormal;   // 법선

in vec2 VertexTexCoord; // 텍스처 좌표

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main() {
    // 텍스처 좌표 전달
    TexCoord = VertexTexCoord;
    Normal = normalize(NormalMatrix * vNormal.xyz);
    Position = vec3(ModelViewMatrix * vPosition);

    // 최종 클립 좌표
    gl_Position = MVP * vPosition;
}
