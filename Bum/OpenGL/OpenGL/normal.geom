#version 400

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec4 posView;    // View * Model�� ��ǥ
} gs_in[];

uniform mat4 projection;
uniform float magnitude; // ���� ���� ������

void GenerateNormalLine(int i)
{
    // 1) ���� ������ ��ġ
    gl_Position = projection * gs_in[i].posView;      
    EmitVertex();

    // 2) ���� ��ġ + (���� ���� * magnitude)
    gl_Position = projection * (gs_in[i].posView + vec4(gs_in[i].normal, 0.0) * magnitude); 
    EmitVertex();

    EndPrimitive();
}

void main()
{
    // �ﰢ���� �� ������(0,1,2)�� ���� ������ ����
    GenerateNormalLine(0);
    GenerateNormalLine(1);
    GenerateNormalLine(2);
}
