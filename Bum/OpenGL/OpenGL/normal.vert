#version 400

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
    vec4 posView;    // View * Model�� ��ǥ
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    // ������ Geometry Shader���� �� �����̹Ƿ� ���⼭�� View * Model������ ����
    vs_out.posView = view * model * vec4(aPos, 1.0);

    // ��� ��� ���
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vs_out.normal = normalize(normalMatrix * aNormal);

    // �� Vertex Shader ��ü�� ȭ�鿡 �׸��� ��ü(�� ��)�� �������Ѵٸ�
    // gl_Position�� �̸� projection * view * model�� �ٲ� ����ؾ� �մϴ�.
    // ���⼭�� "���� ���� �ð�ȭ"�� �����ϰ� �ִٰ� �����ϹǷ�,
    // gl_Position�� ���� �������� �ʾƵ� �˴ϴ�.
    // ������ ������ ������ ���� ����:
    // gl_Position = projection * vs_out.posView;
    // ���� "��ü ��ü"�� �������� �ٸ� ���̴��� �ִٸ� �� VS�� ���� '���� �ð�ȭ��'���θ� ���.
}
