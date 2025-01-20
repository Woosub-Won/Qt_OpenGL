#version 400

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
    vec4 posView;    // View * Model된 좌표
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    // 투영은 Geometry Shader에서 할 예정이므로 여기서는 View * Model까지만 적용
    vs_out.posView = view * model * vec4(aPos, 1.0);

    // 노멀 행렬 계산
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vs_out.normal = normalize(normalMatrix * aNormal);

    // 이 Vertex Shader 자체가 화면에 그리는 물체(구 등)를 렌더링한다면
    // gl_Position을 미리 projection * view * model로 바꿔 출력해야 합니다.
    // 여기서는 "법선 벡터 시각화"만 집중하고 있다고 가정하므로,
    // gl_Position을 굳이 세팅하지 않아도 됩니다.
    // 하지만 보통은 다음과 같이 세팅:
    // gl_Position = projection * vs_out.posView;
    // 만약 "물체 본체"를 렌더링할 다른 셰이더가 있다면 이 VS는 오직 '법선 시각화용'으로만 사용.
}
