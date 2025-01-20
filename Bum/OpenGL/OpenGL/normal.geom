#version 400

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec4 posView;    // View * Model된 좌표
} gs_in[];

uniform mat4 projection;
uniform float magnitude; // 법선 벡터 스케일

void GenerateNormalLine(int i)
{
    // 1) 원래 꼭지점 위치
    gl_Position = projection * gs_in[i].posView;      
    EmitVertex();

    // 2) 원래 위치 + (법선 벡터 * magnitude)
    gl_Position = projection * (gs_in[i].posView + vec4(gs_in[i].normal, 0.0) * magnitude); 
    EmitVertex();

    EndPrimitive();
}

void main()
{
    // 삼각형의 각 꼭지점(0,1,2)에 대해 선분을 생성
    GenerateNormalLine(0);
    GenerateNormalLine(1);
    GenerateNormalLine(2);
}
