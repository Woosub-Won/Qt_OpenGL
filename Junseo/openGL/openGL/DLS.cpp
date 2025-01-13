#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <iostream>
#include <fstream>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#define WIDTH 800
#define HEIGHT 600
#include <glm/gtc/type_ptr.hpp>
#include <vector>
// torus 생성을 위한 구조체와 함수

struct Vertex {
    float x, y, z;        // 위치
    float nx, ny, nz;     // 법선
};

// 전역 변수 추가 (파일 상단에)
float xRot = 0.0f;
float yRot = 0.0f;
int lastX = 0;
int lastY = 0;
bool mouseDown = false;

// 마우스 버튼 콜백 함수
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = true;
            lastX = x;
            lastY = y;
        }
        else if (state == GLUT_UP) {
            mouseDown = false;
        }
    }
}

// 마우스 움직임 콜백 함수
void motion(int x, int y) {
    if (mouseDown) {
        int deltaX = x - lastX;
        int deltaY = y - lastY;

        xRot += deltaY * 0.5f;
        yRot += deltaX * 0.5f;

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}


GLuint numIndices;

std::vector<Vertex> generateTorus(float R, float r, int segments, int sides) {
    std::vector<Vertex> vertices;

    // 각 segment와 side에 대해
    for (int i = 0; i <= segments; i++) {
        float u = 2.0f * glm::pi<float>() * i / segments;

        for (int j = 0; j <= sides; j++) {
            float v = 2.0f * glm::pi<float>() * j / sides;

            // torus 표면상의 점 계산
            float x = (R + r * cos(v)) * cos(u);
            float y = (R + r * cos(v)) * sin(u);
            float z = r * sin(v);

            // 법선 벡터 계산
            float nx = cos(v) * cos(u);
            float ny = cos(v) * sin(u);
            float nz = sin(v);

            // 버텍스 추가
            Vertex vertex = { x, y, z, nx, ny, nz };
            vertices.push_back(vertex);
        }
    }

    return vertices;
}

// 인덱스 버퍼 생성
std::vector<GLuint> generateTorusIndices(int segments, int sides) {
    std::vector<GLuint> indices;

    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < sides; j++) {
            GLuint current = i * (sides + 1) + j;
            GLuint next = current + sides + 1;

            // 두 개의 삼각형 추가
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(current + 1);
        }
    }

    return indices;
}

GLuint createShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile);
char* loadShaderAsString(std::string fileName)
{
    // Initialize input stream.
    std::ifstream inFile(fileName.c_str(), std::ios::binary);

    // Determine shader file length and reserve space to read it in.
    inFile.seekg(0, std::ios::end);
    int fileLength = inFile.tellg();
    char* fileContent = (char*)malloc((fileLength + 1) * sizeof(char));

    // Read in shader file, set last character to NUL, close input stream.
    inFile.seekg(0, std::ios::beg);
    inFile.read(fileContent, fileLength);
    fileContent[fileLength] = '\0';
    inFile.close();

    return fileContent;
}

GLuint programHandle;
GLuint vaoHandle;
void Render();

void checkCompileStatus(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar* info = new GLchar[length + 1];
        glGetShaderInfoLog(shader, length, NULL, info);
        fprintf(stderr, "Compile error: %s\n", info);
        delete[] info;
    }
}

int main(int argc, char** argv)
{
    // GLUT 초기화
    glutInit(&argc, argv); // GLUT 라이브러리 초기화, 필수
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // GLUT_DEPTH 추가
    glutInitWindowPosition(0, 0); // 윈도우 생성 위치
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OpenGL Window"); // 윈도우 생성

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화
    glEnable(GL_CULL_FACE);   // 후면 컬링 활성화

    programHandle = createShaderProgram("shaders/DLS.vert", "shaders/DLS.frag");
    glutDisplayFunc(Render);
    glutMouseFunc(mouse);        // 마우스 버튼 콜백 등록
    glutMotionFunc(motion);      // 마우스 움직임 콜백 등록

    glutMainLoop();

    return 0;
}



GLuint createShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {

    // 1. 셰이더 파일 읽기
    GLchar* vertexShaderSource = loadShaderAsString(vertexShaderFile);
    GLchar* fragmentShaderSource = loadShaderAsString(fragmentShaderFile);

    // 2. 셰이더 객체 생성
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 3. 소스 코드 설정
    glShaderSource(vertShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);

    // 4. 컴파일
    glCompileShader(vertShader);
    glCompileShader(fragShader);
    checkCompileStatus(vertShader);
    checkCompileStatus(fragShader);

    // 5. 프로그램 생성 및 링크
    GLuint Temp = glCreateProgram();
    if (0 == Temp)
    {
        fprintf(stderr, "Error creating program object.\n");
        exit(1);
    }
    glAttachShader(Temp, vertShader);
    glAttachShader(Temp, fragShader);
    glLinkProgram(Temp);

    GLint status;
    glGetProgramiv(Temp, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        fprintf(stderr, "Failed to link shader program!\n");
        GLint logLen;
        glGetProgramiv(Temp, GL_INFO_LOG_LENGTH,
            &logLen);
        if (logLen > 0)
        {
            char* log = (char*)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(Temp, logLen,
                &written, log);
            fprintf(stderr, "Program log: \n%s", log);
            free(log);
        }
        else
        {
            glUseProgram(Temp);
        }
    }

    glBindAttribLocation(Temp, 0, "VertexPosition");
    glBindAttribLocation(Temp, 1, "VertexNormal");

    // torus 생성
    // torus 생성 (createShaderProgram 함수에서)
    // torus 생성 (R: 주 반지름, r: 단면 반지름)
    std::vector<Vertex> vertices = generateTorus(1.0f, 0.5f, 40, 40);
    std::vector<GLuint> indices = generateTorusIndices(40, 40);
    numIndices = indices.size();

    // VBO 생성 및 데이터 전송
    GLuint vboHandles[2];
    glGenBuffers(2, vboHandles);

    // 위치와 법선 데이터
    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // 인덱스 버퍼
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // VAO 설정
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);  // IBO 바인딩 확인
    // 위치 속성 설정
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // 법선 속성 설정
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    numIndices = indices.size();  // 전역 변수로 선언 필요

    return Temp;
}



float angle = 0.0f;  // 회전 각도
void Render() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programHandle);

    // 모델, 뷰, 프로젝션 행렬 설정 (이전과 동일)
    glm::mat4 model = glm::mat4(1.0f);

    float camX = 5.0f * cos(glm::radians(yRot)) * cos(glm::radians(xRot));
    float camY = 5.0f * sin(glm::radians(xRot));
    float camZ = 5.0f * sin(glm::radians(yRot)) * cos(glm::radians(xRot));

    glm::mat4 view = glm::lookAt(
        glm::vec3(camX, camY, camZ),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    glm::mat4 modelView = view * model;
    glm::mat4 mvp = projection * modelView;
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelView)));

    // 행렬 uniform 설정
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelView));
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(programHandle, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix3fv(glGetUniformLocation(programHandle, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // 광원 설정 - view space로 변환
    glm::vec4 worldLightPos = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
    glm::vec4 viewLightPos = view * worldLightPos;
    glUniform4fv(glGetUniformLocation(programHandle, "LightPosition"), 1, glm::value_ptr(viewLightPos));

    // 광원 강도를 높임
    glm::vec3 lightIntensity = glm::vec3(1.0f);
    glUniform3fv(glGetUniformLocation(programHandle, "LightIntensity"), 1, glm::value_ptr(lightIntensity));

    // 재질 속성 - 더 밝게 조정
    glUniform3fv(glGetUniformLocation(programHandle, "Ka"), 1, glm::value_ptr(glm::vec3(0.2f)));    // 주변광
    glUniform3fv(glGetUniformLocation(programHandle, "Kd"), 1, glm::value_ptr(glm::vec3(0.8f)));    // 확산광
    glUniform3fv(glGetUniformLocation(programHandle, "Ks"), 1, glm::value_ptr(glm::vec3(0.2f)));    // 반사광
    glUniform1f(glGetUniformLocation(programHandle, "Shininess"), 100.0f);

    // 렌더링
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
}