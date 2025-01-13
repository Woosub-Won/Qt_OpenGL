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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0); // 윈도우 생성 위치
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OpenGL Window"); // 윈도우 생성
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    programHandle = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    
    glutDisplayFunc(Render);
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
    glBindAttribLocation(Temp, 1, "VertexColor");

    float positionData[] = {
         -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.0f, 0.8f, 0.0f };

    float colorData[] = {
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f };

    GLuint vboHandles[2];
    glGenBuffers(2, vboHandles);
    GLuint positionBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];


    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData,
        GL_STATIC_DRAW);
    // Populate the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData,
        GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    // Enable the vertex attribute arrays
    glEnableVertexAttribArray(0); // Vertex position
    glEnableVertexAttribArray(1); // Vertex color
    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
        (GLubyte*)NULL);
    // Map index 1 to the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
        (GLubyte*)NULL);

    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return Temp;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programHandle); // shader program 사용
    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}