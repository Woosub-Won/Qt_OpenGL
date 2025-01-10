#version 400 core
 
in vec3 color;
in vec2 texCord;
in vec3 normal;
in vec3 curPos;

out vec4 FragColor;

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec4 lightColor;

uniform vec3 cameraPos;


void main()
{ 
	float ambient = 0.2f;

	vec3 lightDir = normalize(lightPos - curPos);
	float diffuse = max( dot( lightDir, normal) , 0.0f);

	float specularLight = 0.5f;
	vec3 viewDir = normalize(cameraPos - curPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float specularPow = pow(max(dot(viewDir, reflectDir), 0.0f),8);
	float specular = specularLight * specularPow;


	FragColor = texture(tex0, texCord) * lightColor * ( diffuse + ambient + specular);
}