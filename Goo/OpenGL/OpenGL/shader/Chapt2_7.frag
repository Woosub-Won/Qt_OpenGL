#version 400

in vec3 FrontColor;
in vec3 BackColor;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

void main(){
	const float scale = 15.0f;
	
	// greaterThan :
	// 첫번째 매개변수의 각 요소가 두번째 매개변수의 각 요소보다 크면 true
	// 매개변수 :
	// 1. 비교할 대상 (벡터형)
	// 2. 기준 대상 (벡터형)

	// fract : 벡터나 스칼라의 소수점 아래 부분만 반환함
	// 매개변수 :
	// 1. 소수점 아래 부분을 구하고자 하는 변수
	bvec2 toDiscard = greaterThan(fract(TexCoord*scale), vec2(0.2f, 0.2f));

	// 불리언 벡터의 모든 요소가 true 인지 확인하는 함수
	// 매개변수 : 
	// 1. 확인하고자 하는 불리언 벡터
	if(all(toDiscard)){
		discard;
	}

	//gl_Position 처럼 GLSL의 내장 변수임
	if(gl_FrontFacing){
		FragColor = vec4(FrontColor, 1.0f);
	}
	else{
		FragColor = vec4(BackColor, 1.0f);
	}
}