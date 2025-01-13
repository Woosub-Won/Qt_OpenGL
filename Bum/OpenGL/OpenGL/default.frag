#version 400 core
 
in vec3 curPos;
in vec3 normal;
in vec3 color;
in vec2 texCord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

struct LightInfo
{
	vec3 lightPos;
	float padding;
	vec4 lightColor;
};

uniform LightInfo lights[2];
   
uniform vec3 camPos;

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
	FragColor = spotLight();
}