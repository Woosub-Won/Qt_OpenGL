#version 400 core
 
in vec3 curPos;
in vec3 normal;
in vec3 color;
in vec2 texCord;

out vec4 FragColor;
out vec4 FrontColor;
out vec4 BackColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

struct LightInfo
{
	vec3 lightPos;
	float padding;
	vec4 lightColor;
};
<<<<<<< HEAD

uniform LightInfo lights[2];
=======
uniform LightInfo lights[2];

struct FogInfo {
 float maxDist;
 float minDist;
 vec3 padding;
 vec3 color;
};
uniform FogInfo Fog;
>>>>>>> origin/bum
   
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

	 
	 Color += texture(tex0, texCord) * (  ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
	}
	return Color;
}
vec4 directLight()
{
<<<<<<< HEAD
vec4 Color = vec4(0,0,0,0);
	for(int i= 0; i <2 ;i++)
	{
	//ambient
	float ambient = 0.2f;

	//diffuse
	vec3 n_normal = normalize(normal);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);
	
	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(camPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);
	
	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;

	  
	Color += texture(tex0, texCord) * (  ambient + diffuse + specular) * lights[i].lightColor;
=======
	vec4 Color = vec4(0,0,0,0);
	for(int i= 0; i < 2 ;i++)
	{
		//ambient
		float ambient = 0.2f;

		//diffuse
		vec3 n_normal = normalize(normal);
		
		vec3 lightDir = vec3(0,0,0);

		if( i == 0 )
			lightDir = normalize(vec3(0.0f, 1.0f, 0.0f));
		if( i == 1 )
			lightDir = normalize(vec3(0.0f, -1.0f, 0.0f));
		 
		float diffuse = 0.0f;
		
		if(gl_FrontFacing)
			diffuse = max( dot( lightDir, n_normal) , 0.0f);
		else
			diffuse = max( dot( lightDir, -n_normal) , 0.0f);
		
		//specular
		float specularLight = 0.5f;
		vec3 viewDir = normalize(camPos - curPos);
		vec3 reflectDir = reflect(-lightDir, n_normal);
		
		float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
		float specular = specularLight * specularPow;

		  
		Color += texture(tex0, texCord) * (  ambient + diffuse + specular) * lights[i].lightColor;
>>>>>>> origin/bum
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

	//return texture(tex0, texCord) * lights[i].lightColor * (diffuse + ambient + specular);
	//return texture(tex0, texCord) * (  ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
	Color +=  (ambient + diffuse *inten + specular * inten) * lights[i].lightColor;
 }
 return Color;
}
void main()
{ 
	 
	//FragColor = pointLight();
	//FragColor = directLight();
	//FragColor = spotLight(); 

	//float dist = length(camPos - curPos);
	//float fogFactor  = (Fog.maxDist - dist) / (Fog.maxDist - Fog.minDist);
	//fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	//
	//vec4 shadeColor = spotLight();
	//vec3 color = mix( Fog.color, shadeColor.xyz, fogFactor );
	//FragColor = vec4(color, fogFactor);

	float density = 0.5f;
	float dist = length(camPos - curPos);
	float fogFactor = exp(-pow(density * dist,2));
	
	vec4 shadeColor = spotLight();
	vec3 finalColor = mix(Fog.color, shadeColor.xyz, fogFactor);
	FragColor = vec4(finalColor, 1.0f);
}