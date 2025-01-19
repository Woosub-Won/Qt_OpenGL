#version 400 core
layout(location=0) in vec3 aPos;

// 스카이박스는 카메라의 회전만 반영하기 위해
// view 행렬에서 translate 부분을 제거 (3x3 or 4x4에서 상단3x3만 사용)
uniform mat4 uProjection;
uniform mat4 uView;

out vec3 TexCoords;

void main()
{
    // 스카이박스는 view의 회전만 반영 -> 위치 이동은 무시
    mat3 rot = mat3(uView);
    vec3 dir = rot * aPos;

    TexCoords = aPos;
    // TexCoords = dir;
    gl_Position = uProjection * vec4(dir, 1.0);
}
