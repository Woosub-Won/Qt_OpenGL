#version 400

in vec3 FrontColor;
in vec3 BackColor;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

void main(){
	const float scale = 15.0f;
	
	// greaterThan :
	// ù��° �Ű������� �� ��Ұ� �ι�° �Ű������� �� ��Һ��� ũ�� true
	// �Ű����� :
	// 1. ���� ��� (������)
	// 2. ���� ��� (������)

	// fract : ���ͳ� ��Į���� �Ҽ��� �Ʒ� �κи� ��ȯ��
	// �Ű����� :
	// 1. �Ҽ��� �Ʒ� �κ��� ���ϰ��� �ϴ� ����
	bvec2 toDiscard = greaterThan(fract(TexCoord*scale), vec2(0.2f, 0.2f));

	// �Ҹ��� ������ ��� ��Ұ� true ���� Ȯ���ϴ� �Լ�
	// �Ű����� : 
	// 1. Ȯ���ϰ��� �ϴ� �Ҹ��� ����
	if(all(toDiscard)){
		discard;
	}

	//gl_Position ó�� GLSL�� ���� ������
	if(gl_FrontFacing){
		FragColor = vec4(FrontColor, 1.0f);
	}
	else{
		FragColor = vec4(BackColor, 1.0f);
	}
}