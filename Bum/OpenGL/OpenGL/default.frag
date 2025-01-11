#version 400 core
 
in vec3 color;
in vec2 texCord;
in vec3 normal;
in vec3 curPos;

out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec3 lightPos;
uniform vec4 lightColor;

uniform vec3 cameraPos;

vec4 pointLight()
{	
	//ambient
	float ambient = 0.2f;
	
	//diffuse
	vec3 lightVec = lightPos - curPos;
	float dist = length(lightVec);
	float a = 3.0f;
	float b = 0.7f;
	float inten = 1.0f / ( a * dist * dist + b * dist + 1.0f);

	vec3 n_normal = normalize(normal);

	vec3 lightDir = normalize(lightVec);
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);

	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(cameraPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);

	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;

	 
	//return texture(tex0, texCord) * lightColor * (diffuse + ambient + specular);
	return texture(tex0, texCord) * (  ambient + diffuse *inten + specular * inten) * lightColor;
}

vec4 directLight()
{

	//ambient
	float ambient = 0.2f;

	//diffuse
	vec3 n_normal = normalize(normal);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);
	
	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(cameraPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);
	
	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;

	  
	return texture(tex0, texCord) * (  ambient + diffuse + specular) * lightColor;
}

vec4 spotLight() 
{
	//ambient
	float ambient = 0.2f;
	
	//diffuse 

	vec3 n_normal = normalize(normal);

	vec3 lightDir = normalize(lightPos - curPos);
	float diffuse = max( dot( lightDir, n_normal) , 0.0f);

	//specular
	float specularLight = 0.5f;
	vec3 viewDir = normalize(cameraPos - curPos);
	vec3 reflectDir = reflect(-lightDir, n_normal);

	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	float specular = specularLight * specularPow;
	
	float outerCone = 0.90f; //cos value
	float innerCone = 0.95f; //cos value
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float epsilon = innerCone - outerCone;

	float inten = clamp((angle - outerCone) / epsilon, 0.0f, 1.0f);

	//return texture(tex0, texCord) * lightColor * (diffuse + ambient + specular);
	return texture(tex0, texCord) * (  ambient + diffuse *inten + specular * inten) * lightColor;
 
}
void main()
{ 
	 
	//FragColor = pointLight();
	//FragColor = directLight();
	FragColor = spotLight();
}