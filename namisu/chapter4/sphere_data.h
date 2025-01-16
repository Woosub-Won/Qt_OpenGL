#ifndef SPHERE_DATA_H
#define SPHERE_DATA_H

#include <vector>
#include <cmath>
#include <C:/glm-master/glm/glm.hpp>
#include <C:/glm-master/glm/gtc/matrix_transform.hpp>
#include <C:/glm-master/glm/gtc/type_ptr.hpp>

std::vector<float> generateSphereVertices(float radius, int stacks, int slices) {
    std::vector<float> vertices;
    const float PI = 3.14159265359f;

    for (int i = 0; i < stacks; ++i) {
        float stackAngle1 = PI / 2 - i * (PI / stacks); // 위도 1
        float stackAngle2 = PI / 2 - (i + 1) * (PI / stacks); // 위도 2

        float xy1 = radius * cos(stackAngle1);
        float z1 = radius * sin(stackAngle1);
        float xy2 = radius * cos(stackAngle2);
        float z2 = radius * sin(stackAngle2);

        for (int j = 0; j < slices; ++j) {
            float sliceAngle1 = j * (2 * PI / slices);
            float sliceAngle2 = (j + 1) * (2 * PI / slices);

            // 정점 계산
            glm::vec3 p1(xy1 * cos(sliceAngle1), xy1 * sin(sliceAngle1), z1); // 스택1-슬라이스1
            glm::vec3 p2(xy2 * cos(sliceAngle1), xy2 * sin(sliceAngle1), z2); // 스택2-슬라이스1
            glm::vec3 p3(xy2 * cos(sliceAngle2), xy2 * sin(sliceAngle2), z2); // 스택2-슬라이스2
            glm::vec3 p4(xy1 * cos(sliceAngle2), xy1 * sin(sliceAngle2), z1); // 스택1-슬라이스2

            // 삼각형 1 (반시계 방향)
            vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p1.x, p1.y, p1.z});
            vertices.insert(vertices.end(), {p2.x, p2.y, p2.z, p2.x, p2.y, p2.z});
            vertices.insert(vertices.end(), {p3.x, p3.y, p3.z, p3.x, p3.y, p3.z});

            // 삼각형 2 (반시계 방향)
            vertices.insert(vertices.end(), {p1.x, p1.y, p1.z, p1.x, p1.y, p1.z});
            vertices.insert(vertices.end(), {p3.x, p3.y, p3.z, p3.x, p3.y, p3.z});
            vertices.insert(vertices.end(), {p4.x, p4.y, p4.z, p4.x, p4.y, p4.z});
        }
    }

    return vertices;
}



std::vector<unsigned int> generateSphereIndices(int stacks, int slices) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return indices;
}

#endif // SPHERE_DATA_H
