#pragma once

#include <vgl.h>
#include <vec.h>

struct PolVertex {
	vec4 position;
	vec4 color;
};

class MyPolygon {
public:
	MyPolygon(void) {

	}
	~MyPolygon(void) {

	}

	GLuint vao;
	GLuint vbo;
	static const int NumVertices = 3;
	//static const int NumVertices = 1;
	GLuint Init() {
		vec4 vertex_pos[3] = {
			vec4(-0.8f, -0.8f, 0.0f, 1.0f),
			vec4(0.8f, -0.8f, 0.0f, 1.0f),
			vec4(0.0f, 0.8f, 0.0f, 1.0f)
		};
		vec4 color_data[3] = {
			vec4(1.0f, 0.0f, 0.0f, 1.0f),
			vec4(0.0f, 1.0f, 0.0f, 1.0f),
			vec4(0.0f, 0.0f, 1.0f, 1.0f)
		};

		PolVertex vertices[3];
		vertices[0].position = vertex_pos[0]; vertices[0].color = color_data[0];
		vertices[1].position = vertex_pos[1]; vertices[1].color = color_data[1];
		vertices[2].position = vertex_pos[2]; vertices[2].color = color_data[2];

		/*vec4 vertex_pos[1] = {
			vec4(0.0f, 0.8f, 0.0f, 1.0f)
		};
		vec4 color_data[1] = {
			vec4(1.0f, 0.0f, 0.0f, 1.0f)
		};

		PolVertex vertices[1];
		vertices[0].position = vertex_pos[0]; vertices[0].color = color_data[0];*/

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
		return vao;
	}

	void SetPositionAndColorAttribute(GLuint prog) {
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PolVertex), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PolVertex), BUFFER_OFFSET(sizeof(vec4)));
		glEnableVertexAttribArray(1);
	}

	void draw(GLuint prog) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		SetPositionAndColorAttribute(prog);

		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		//glPointSize(20.0f);
		//glDrawArrays(GL_POINTS, 0, NumVertices);
	}

};