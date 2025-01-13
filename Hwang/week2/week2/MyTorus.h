#pragma once

#include <vector>
#include <math.h>
#include <vgl.h>

GLuint vao, vbo;

void generateTorus(std::vector<float>& vertices, std::vector<float>& normals, float innerRadius, float outerRadius, int nsides, int rings) {
    float ringFactor = 2.0f * M_PI / rings;
    float sideFactor = 2.0f * M_PI / nsides;

    for (int ring = 0; ring < rings; ++ring) {
        float u = ring * ringFactor;
        float cosU = cos(u);
        float sinU = sin(u);

        for (int side = 0; side < nsides; ++side) {
            float v = side * sideFactor;
            float cosV = cos(v);
            float sinV = sin(v);

            float r = outerRadius + innerRadius * cosV;

            // Position
            float x = r * cosU;
            float y = r * sinU;
            float z = innerRadius * sinV;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal
            float nx = cosV * cosU;
            float ny = cosV * sinU;
            float nz = sinV;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }
}

void initTorus() {
    // Generate Torus vertices and normals
    std::vector<float> vertices;
    std::vector<float> normals;
    generateTorus(vertices, normals, 0.2f, 0.5f, 30, 30);

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    size_t vertexSize = vertices.size() * sizeof(float);
    size_t normalSize = normals.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexSize + normalSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize, normals.data());

    // Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexSize));
    glEnableVertexAttribArray(1);
}

void drawTorus() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 30 * 30 * 6);
}