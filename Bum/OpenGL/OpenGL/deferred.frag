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
uniform sampler2D shadowMap;
uniform mat4 shadowMatrix;

//imgui
uniform bool bFogEffect;
           

vec4 directLight(vec4 pos, vec4 normal, vec4 myColor)
{
	vec4 Color = vec4(0,0,0,0);

		//ambient
		float ambient = 0.1f;

		//diffuse
		vec3 n_normal = normalize(normal.xyz);
		
		vec3 lightDir = vec3(0,0,0);

		lightDir = normalize(vec3(0.0f, -1.0f, 0.0f));
		 
		float diffuse = 0.0f;
		diffuse = max(dot(lightDir, -n_normal), 0.0f);

		//specular
		vec3 curPos = pos.xyz;

		float specularLight = 0.5f;
		vec3 viewDir = normalize(camPos - curPos);
		vec3 reflectDir = reflect(-lightDir, n_normal);
		
		float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
		float specular = specularLight * specularPow;

		vec4 lightColor = vec4(1,1,1,1);
		  
		//Color += texture(tex0, texCoord) * (  ambient + diffuse + specular) * lights[i].lightColor;
		Color += (ambient + diffuse  + specular) * lightColor;
	 

	return Color;
}

vec4 spotLight(vec4 pos, vec4 normal, vec4 myColor) 
{
	vec4 Color = vec4(0,0,0,0);  // = myColor;
	
	vec3 n_normal = normalize(normal.xyz);
	vec3 viewDir = normalize(camPos - pos.xyz);
	
	for(int i = 0; i < 2; i++)
	{	
		vec3 lightDir = normalize(lights[i].lightPos - pos.xyz);
		vec3 reflectDir = reflect(-lightDir, n_normal);
		
		//ambient
		float ambient = 0.1f;
		
		//diffuse 
		float outerCone = 0.90f;
		float innerCone = 0.95f; 
		float diffuse = max( dot( lightDir, n_normal) , 0.0f);

		//specular
		float specularLight = 0.5f; 
		float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
		float specular = specularLight * specularPow;

		float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
		float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);


		Color += (ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
	}
	return Color;
}
   
void main()
{    
	vec4 pos = texture(PositionTex, texCoord);
	vec4 nor = texture(NormalTex, texCoord);
	vec4 myColor = texture(ColorTex,texCoord); 
	
	vec4 spotLightColor = spotLight(pos, nor, myColor);
	vec4 directLight = directLight(pos, nor, myColor); 
	//FragColor = myColor * (spotLightColor);   
	
	float shadowFactor = 1.0f;
	vec4 lightScreen = shadowMatrix * pos;
	lightScreen /= lightScreen.w;
	lightScreen.z = (lightScreen.z + 1.0) * 0.5;
	 
	vec2 shadowCoords = lightScreen.xy;
	shadowCoords += 1.0;
	shadowCoords *= 0.5f;
		
	float depth = texture(shadowMap, shadowCoords.xy).r;
	float bias = 0.005f;
	
	if(depth + bias <= lightScreen.z)
		shadowFactor = 0.2f;
	
	//FragColor = myColor * (spotLightColor + directLight * 0.5f) * shadowFactor; 
	FragColor = myColor * (spotLightColor + directLight * 0.5f) * shadowFactor; 

	//FragColor = vec4(depth);
	//FragColor = vec4(lightScreen.z);
	//FragColor = vec4(shadowFactor);
}
