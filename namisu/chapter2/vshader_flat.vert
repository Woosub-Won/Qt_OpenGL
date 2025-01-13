#version 400

layout(location = 0) in vec3 VertexPosition; // 정점 위치 입력
layout(location = 1) in vec3 VertexNormal;   // 정점 법선 벡터 입력

flat out vec3 LightIntensity; // flat 키워드를 추가하여 조명 색상을 보간하지 않음

uniform mat4 ModelViewMatrix;  // 모델-뷰 변환 행렬
uniform mat3 NormalMatrix;     // 법선 변환 행렬
uniform mat4 ProjectionMatrix; // 투영 행렬
uniform mat4 MVP;              // 모델-뷰-투영 행렬

struct LightInfo {
    vec4 Position; // 광원의 위치
    vec3 La;       // 주변광(Ambient) 강도
    vec3 Ld;       // 난반사광(Diffuse) 강도
    vec3 Ls;       // 반사광(Specular) 강도
};

uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka; // 주변광 반사율
    vec3 Kd; // 난반사 반사율
    vec3 Ks; // 반사광 반사율
    float Shininess; // 반사광 샤이니니스 값
};

uniform MaterialInfo Material;

void main() {
    vec3 tnorm = normalize(NormalMatrix * VertexNormal); // 변환된 정점 법선 벡터
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0); // 뷰 좌표계에서 정점 좌표
    vec3 s = normalize(vec3(Light.Position - eyeCoords)); // 광원의 방향
    vec3 v = normalize(-eyeCoords.xyz); // 시점 방향

    vec3 r = reflect(-s, tnorm); // 반사 벡터
    vec3 ambient = Light.La * Material.Ka; // 주변광 계산
    float sDotN = max(dot(s, tnorm), 0.0); // 광원 방향과 법선 벡터의 내적
    vec3 diffuse = Light.Ld * Material.Kd * sDotN; // 난반사광 계산
    vec3 spec = vec3(0.0);

    if (sDotN > 0.0) {
        spec = Light.Ls * Material.Ks *
               pow(max(dot(r, v), 0.0), Material.Shininess); // 반사광 계산
    }

    LightIntensity = ambient + diffuse + spec; // 조명 색상 계산
    gl_Position = MVP * vec4(VertexPosition, 1.0); // 정점 위치 변환
}
