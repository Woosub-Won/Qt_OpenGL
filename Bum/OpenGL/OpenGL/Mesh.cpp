#include "Mesh.h" 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.inl>


Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
	Init(vertices, indices, textures);
}

void Mesh::Init(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices; 
	Mesh::textures = textures;

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}


void Mesh::Draw(Shader& shader, Camera& camera)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numTex = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(++numDiffuse);
		}
		else if (type == "specular")
		{
			num = std::to_string(++numSpecular);
		}
		else if (type == "tex")
		{
			num = std::to_string(++numTex);

		}
		textures[i].texUnit(shader, (type + num).c_str(), i + 1);
		textures[i].Bind();
	}

	// Cubemap  
	GLuint texUni = glGetUniformLocation(shader.ID, "skybox");
	shader.Activate();
	glUniform1i(texUni, 10);

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader);  

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);		
}

void Mesh::Draw_Shadow(Shader& shader, Camera& camera,vec3 lightPos)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();	
	
	//mat4 view = glm::lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, 1));
	mat4 view = glm::lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, 1));
	mat4 proj = glm::perspective(glm::radians(camera.GetFOV()), (float)(camera.GetWidth()) / camera.GetHeight(), camera.GetNearPlane(), camera.GetFarPlane());	
	
	mat4 bias(
		vec4(0.5, 0, 0, 0.5),
		vec4(0, 0.5, 0, 0.5),
		vec4(0, 0, 0.5, 0.5),
		vec4(0, 0, 0, 1.0)
	);
	mat4 shadowMatrix = bias * proj * view;

	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, glm::value_ptr(camera.GetView()));
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(proj));
	
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "shadowMatrix"), 1, false, glm::value_ptr(shadowMatrix));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}

void Mesh::Translate(Shader& shader, vec3 basePosition,  vec3 Translate)
{
	shader.Activate();

	mat4 model = mat4(1.0f);
	model = translate(model, basePosition +Translate);

	SetMatrixUniform(shader, "model", model);
}

void Mesh::UpdateVPMatrix(Shader& shader, Camera& camera)
{
}
