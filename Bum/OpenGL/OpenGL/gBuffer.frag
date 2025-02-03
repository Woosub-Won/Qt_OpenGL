#version 400 core
 
in vec3 curPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
in vec3 MapReflectDir;
in vec3 MapRefractDir;
in vec4 ProjTexCoord; 
 
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColorData; 


 
struct LightInfo
{
	vec3 lightPos;
	float padding;
	vec4 lightColor;
};
uniform LightInfo lights[2];

struct FogInfo {
 float maxDist;
 float minDist;
 vec3 padding;
 vec3 color;
};
uniform FogInfo Fog;  
uniform samplerCube skybox;
uniform vec3 camPos;

const int levels = 3;
const float scaleFactor = 1.0 / levels;
    
uniform sampler2D PositionTex, NormalTex, ColorTex; 
uniform sampler2D tex0;
uniform sampler2D tex1; 


//imgui
uniform bool bFogEffect;
  
   
void main()
{  

	FragColor =  vec4(1,0,0,1); 
	PositionData = curPos;
	NormalData = normal; 
	vec4 frogColor = texture(tex1, texCoord);
	
	if(frogColor.a == 0) discard;
	
    vec4 ReflectColor = texture(skybox, MapReflectDir);
	vec4 RefractColor = texture(skybox, MapRefractDir);
	
	
	vec4 projTexColor = vec4(0.0);
	if( ProjTexCoord.z > 0.0 )
		projTexColor = textureProj(tex1,ProjTexCoord);
	 
	 
	ColorData =   ReflectColor.xyz + projTexColor.xyz * 0.5;//ReflectColor.xyz; //frogColor.xyz; 
	
}
