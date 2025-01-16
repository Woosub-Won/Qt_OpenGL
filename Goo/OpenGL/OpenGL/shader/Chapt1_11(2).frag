#version 400

in vec3 TexCoord;
layout (location = 0) out vec4 FragColor;

// std140 layout : 셰이더 변수의 메모리 배치를 16바이트 단위로 정렬하는 규칙을 따름
// 이는 GLSL 표준에 맞는 데이터 정렬 방식임
// layout은 데이터의 배치를 명시하는데 사용하는 키워드 
layout (std140) uniform BlobSettings{
	vec4 InnerColor;
	vec4 OuterColor;
	float RadiusInner;
	float RadiusOuter;
}Blob;

void main(){
	float dx = TexCoord.x - 0.5f;
	float dy = TexCoord.y - 0.5f;
	float dist = sqrt(dx*dx+dy*dy);

	//smoothstep : 두 값 사이의 어떤 값을 0~1 범위의 값으로 천천히 변화시키는 함수
	// 매개변수 
	// 1. 부드러운 변환을 시작할 경계값
	// 2. 부드러운 변환을 끝낼 경계값
	// 3. 입력값 : 이 값이 두 경계값 범위에서 변할 때 함수의 결과가 0~1로 부드럽게 변함
	FragColor = mix(Blob.InnerColor, Blob.OuterColor, smoothstep(Blob.RadiusInner, Blob.RadiusOuter, dist));
}