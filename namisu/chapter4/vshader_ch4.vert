#version 400
in vec4 vPosition; // 위치
in vec3 vNormal;   // 법선
in vec2 vTexCoord;
in vec4 vTangent;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

out mat3 toObjectLocal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main() {
    // Normal 및 Tangent 변환
    vec3 norm = normalize(NormalMatrix * vNormal);
    vec3 tang = normalize(NormalMatrix * vec3(vTangent.xyz));

    // Binormal 계산
    vec3 binormal = normalize(cross(norm, tang)) * vTangent.w;

    // Tangent Space 매트릭스 구성
    toObjectLocal = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z
    );

    // 출력 변수 설정
    Normal = norm;
    Position = vPosition;
    TexCoord = vec2(vTexCoord.x, 1.0 - vTexCoord.y);

    // 최종 클립 공간 위치 계산
    gl_Position = MVP * vPosition;
}
