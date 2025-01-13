#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct LightInfo
{
	glm::vec3 lightPos;
	float padding; 
	glm::vec4 lightColor;
};