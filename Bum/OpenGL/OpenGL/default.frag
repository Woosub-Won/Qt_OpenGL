#version 400 core
 
in vec3 curPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;
in vec3 MapReflectDir;
in vec3 MapRefractDir;
in vec4 ProjTexCoord;

out vec4 FragColor;
out vec4 FrontColor;
out vec4 BackColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

//imgui
uniform bool bFogEffect;

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
 
vec4 pointLight()
{	
	vec4 Color = vec4( 0,0,0,0);
	for(int i = 0; i < 2; i++)
	{
	//ambient
	float ambient = 0.2f;
	
	//diffuse
	vec3 lightVec = lights[i].lightPos - curPos;
	float dist = length(lightVec);
	float a = 3.0f;
	float b = 0.7f;
	float inten = 1.0f / ( a * dist * dist + b * dist + 1.0f);

	vec3 n_normal = normalize(normal);

	vec3 lightDir = normalize(lightVec);
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);

	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(camPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);

	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;

	vec4 boxColor = texture(tex0, texCoord);
	vec4 frogColor = texture(tex1, texCoord);
	vec4 texColor = mix(boxColor, frogColor, 0.5);
	 
	 Color += texColor* (  ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
	}
	return Color;
}
vec4 directLight()
{
	vec4 Color = vec4(0,0,0,0);

	{
		//ambient
		float ambient = 0.1f;

		//diffuse
		vec3 n_normal = normalize(normal);
		
		vec3 lightDir = vec3(0,0,0);

		lightDir = normalize(vec3(0.0f, -1.0f, 0.0f));
		 
		float diffuse = 0.0f;
		
		//if(gl_FrontFacing)
		//	diffuse = max( dot( lightDir, n_normal) , 0.0f);
		//else
		//	diffuse = max( dot( lightDir, -n_normal) , 0.0f);
		
		diffuse = max(dot(lightDir, -n_normal), 0.0f);

		//specular
		float specularLight = 0.5f;
		vec3 viewDir = normalize(camPos - curPos);
		vec3 reflectDir = reflect(-lightDir, n_normal);
		
		float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
		float specular = specularLight * specularPow;

		vec4 lightColor = vec4(1,1,1,1);
		  
		//Color += texture(tex0, texCoord) * (  ambient + diffuse + specular) * lights[i].lightColor;
		Color += (ambient + diffuse  + specular) * lightColor;
	}

	return Color;
}
vec4 spotLight() 
{
	vec4 Color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for(int i = 0; i < 2; i++)
	{	
	//ambient
	float ambient = 0.2f;
	
	//diffuse 
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	vec3 n_normal = normalize(normal);

	vec3 lightDir = normalize(lights[i].lightPos - curPos);
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);

	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(camPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);

	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);


	//return texture(tex0, texCoord) * lights[i].lightColor * (diffuse + ambient + specular);
	//return texture(tex0, texCoord) * (  ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
	Color += (ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
 }
 return Color;
}
void main()
{  
	vec4 boxColor = texture(tex0, texCoord);
	vec4 frogColor = texture(tex1, texCoord);
	vec4 texColor = vec4(0,0,0,0);
	
	if(frogColor.a < 0.15)
	{	
		
		texColor = boxColor;
		
		//discard;
	}
	 
	else
		texColor = frogColor; 

	vec4 spotLight = spotLight();
	vec4 directLight = directLight(); 
	vec4 Color = (spotLight+ directLight * 0.1f) * texColor;
	Color = texColor;
	
	if(bFogEffect)
	{
		float density = 0.5f;
		float dist = length(camPos - curPos);
		float fogFactor = exp(-pow(density * dist,2));
		
		vec3 finalColor = mix(Fog.color, Color.xyz, fogFactor);
		Color = vec4(finalColor, 1.0f);
	}  

	//reflect
    vec4 ReflectColor = texture(skybox, MapReflectDir);
	vec4 RefractColor = texture(skybox, MapRefractDir);

	//FragColor = mix(ReflectColor, RefractColor, 0.9); 
	//FragColor = mix(FragColor, texColor, 1);

	//FragColor = mix(ReflectColor, projTexColor,  1);
	//FragColor = ProjTexCoord;
	
	vec4 projTexColor = vec4(0.0);
	if( ProjTexCoord.z > 0.0 )
		projTexColor = textureProj(tex1,ProjTexCoord);
	
	FragColor =  ReflectColor + projTexColor * 0.5;
	 
}
