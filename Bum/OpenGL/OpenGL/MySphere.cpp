#include "MySphere.h"

MySphere::MySphere()
{
	bInitialized = false;
	NumVertices = 0;
}

MySphere::~MySphere()
{
}
void MySphere::Init(int la_slice, int lo_slice, std::vector <Texture>& textures, float radius, vec3 color)
{
    // The Cube should be initialized only once
    if (bInitialized == true) return;
      
   // Mesh::textures = textures;

    NumVertices = (la_slice - 2) * lo_slice * 2 * 3 + 2 * lo_slice * 3;
    Vertices.resize(NumVertices);

    float da = 3.141592 / la_slice; // Latitude step
    float db = 3.141592 * 2.0 / lo_slice; // Longitude step
    int cur = 0;

    for (int i = 0; i < la_slice; i++)
    {
        // Calculate current and next latitude circles
        float y1 = cos(da * i);                 // Current latitude height
        float r1 = sqrt(1 - y1 * y1) * radius; // Radius at current latitude
        float y2 = cos(da * (i + 1));           // Next latitude height
        float r2 = sqrt(1 - y2 * y2) * radius; // Radius at next latitude

        for (int j = 0; j < lo_slice; j++)
        {
            // Calculate points for the current quad
            vec3 a(r1 * cos(db * j), y1 * radius, r1 * sin(db * j));
            vec3 b(r2 * cos(db * j), y2 * radius, r2 * sin(db * j));
            vec3 c(r2 * cos(db * (j + 1)), y2 * radius, r2 * sin(db * (j + 1)));
            vec3 d(r1 * cos(db * (j + 1)), y1 * radius, r1 * sin(db * (j + 1)));

            // Texture coordinates
            vec2 ta(1 - j / float(lo_slice), 1 - i / float(la_slice));
            vec2 tb(1 - j / float(lo_slice), 1 - (i + 1) / float(la_slice));
            vec2 tc(1 - (j + 1) / float(lo_slice), 1 - (i + 1) / float(la_slice));
            vec2 td(1 - (j + 1) / float(lo_slice), 1 - i / float(la_slice));

            // Add triangles for the current quad
            if (i != 0)
            {
                Vertices[cur].position = a; Vertices[cur].color = color; Vertices[cur].texUV = ta; Vertices[cur].normal = normalize(a); cur++;
                Vertices[cur].position = d; Vertices[cur].color = color; Vertices[cur].texUV = td; Vertices[cur].normal = normalize(d); cur++;
                Vertices[cur].position = b; Vertices[cur].color = color; Vertices[cur].texUV = tb; Vertices[cur].normal = normalize(b); cur++;
            }

            if (i != la_slice - 1)
            {
                Vertices[cur].position = d; Vertices[cur].color = color; Vertices[cur].texUV = td; Vertices[cur].normal = normalize(d); cur++;
                Vertices[cur].position = c; Vertices[cur].color = color; Vertices[cur].texUV = tc; Vertices[cur].normal = normalize(c); cur++;
                Vertices[cur].position = b; Vertices[cur].color = color; Vertices[cur].texUV = tb; Vertices[cur].normal = normalize(b); cur++;
            }
        }
    }

    // OpenGL Buffer setup
    VAO.Bind();
    VBO VBO(Vertices);
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
    VAO.Unbind();
    VBO.Unbind();

    bInitialized = true;
}


void MySphere::Draw(Shader& shader, Camera& camera)
{
	if (!bInitialized) return;			// check whether it is initiazed or not. 

	Mesh::Draw(shader, camera);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void MySphere::Translate(Shader& shader, vec3 basePosition, vec3 Translate)
{
    Mesh::Translate(shader, basePosition, Translate);
}
