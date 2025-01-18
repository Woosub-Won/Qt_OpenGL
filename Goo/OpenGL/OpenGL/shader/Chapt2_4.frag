#version 400

in vec3 FrontColor;
in vec3 BackColor;

layout (location = 0) out vec4 FragColor;

void main(){
	//gl_Position 처럼 GLSL의 내장 변수임
	if(gl_FrontFacing){
		FragColor = vec4(FrontColor, 1.0f);
	}
	else{
		FragColor = vec4(BackColor, 1.0f);
	}
}