#include "widget.h"
#include "sphere_data.h"
#include "cube_data.h"
#include "cube_data_no_top.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <QTimer> //Qt 타이머 추가

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent), shaderProgram(0), VAO(0), VBO(0), rotationAngle(0.0f) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]() {
        rotationAngle += 1.0f;
        if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
        update();
    });
    timer->start(16);
}

Widget::~Widget() {
    makeCurrent();
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// 파일에서 셰이더 코드를 읽어오는 함수
string readShaderFile(const char* filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open shader file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 셰이더 로드 및 컴파일 함수
GLuint Widget::loadShader(const char* filePath, GLenum shaderType) {
    string shaderCode = readShaderFile(filePath);
    const char* code = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    // 컴파일 상태 확인
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        cerr << "Error compiling shader (" << filePath << "): " << infoLog << endl;
        exit(EXIT_FAILURE);
    }

    return shader;
}

GLuint Widget::InitShader(const char* vertexPath, const char* fragmentPath) {
    GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 연결 상태 확인
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        cerr << "Error linking shader program: " << infoLog << endl;
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Widget::setUniforms(GLuint program, const mat4& model, const mat4& view, const mat4& projection) {
    mat4 mvp = projection * view * model;
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelViewMatrix"), 1, GL_FALSE, value_ptr(view * model));
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "NormalMatrix"), 1, GL_FALSE, value_ptr(normalMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, value_ptr(mvp));
}

void Widget::initializeCubeBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 6, cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void setTwoSidedUniform(GLuint shaderProgram){
    glUniform4f(glGetUniformLocation(shaderProgram, "Light.Position"), 0.0f, 0.0f, 2.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Intensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ka"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Kd"), 0.8f, 0.5f, 0.3f);
}

void setLightUniform(GLuint shaderProgram){
    glUniform4f(glGetUniformLocation(shaderProgram, "Light.Position"), 0.0f, 0.0f, 2.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.La"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Ld"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Ls"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ka"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Kd"), 0.8f, 0.5f, 0.3f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ks"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "Material.Shininess"), 32.0f);
}


void setFlatUniform(GLuint shaderProgram){
    glUniform3f(glGetUniformLocation(shaderProgram, "LightPosition"), 0.0f, 0.0f, 2.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "LightIntensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Ka"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Kd"), 0.8f, 0.5f, 0.3f);
}

void Widget::initializeSphereBuffers(const std::vector<float>& sphereVertices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    // 위치 속성 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 법선 속성 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Widget::loadTexture(const char* filename){
    QImage timg = QImage(filename).convertToFormat(QImage::Format_RGBA8888);

    if (timg.isNull()) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return;
    }

    // 텍스처 생성
    glActiveTexture(GL_TEXTURE0);
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);

    // 텍스처 파라미터 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 텍스처 이미지 설정
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, timg.width(), timg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, timg.bits());

    // 텍스처 유니폼 변수 설정
    glUseProgram(shaderProgram);  // 셰이더 프로그램 활성화
    GLuint texLocation = glGetUniformLocation(shaderProgram, "Tex1");  // 텍스처 유니폼 변수 이름 확인
    if (texLocation >= 0) {
        glUniform1i(texLocation, 0);  // 텍스처 유닛 0에 바인딩된 텍스처를 사용할 것이라고 지정
    } else {
        fprintf(stderr, "Uniform variable Tex1 not found!\n");
    }

    // 텍스처 미핑 생성
    glGenerateMipmap(GL_TEXTURE_2D);
}


void Widget::initializeGL() {
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "GLEW initialized. OpenGL version: " << glGetString(GL_VERSION) << endl;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    shaderProgram = InitShader( "C:/Users/SeunghyunWoo/Documents/chapter4/vshader_ch1.vert",
                               "C:/Users/SeunghyunWoo/Documents/chapter4/fshader_ch1.frag");

    // 유니폼 초기화
    glUseProgram(shaderProgram);
    setLightUniform(shaderProgram);

    loadTexture("C:/ImageTemp/brick1.jpg");
    //setFlatUniform(shaderProgram);

    //auto sphereVertices = generateSphereVertices(1.0f, 15, 15);
    //drawSize = sphereVertices.size();
    //std::cout << "Vertices count: " << sphereVertices.size() << std::endl;
    //initializeSphereBuffers(sphereVertices);
    initializeCubeBuffers();
    //glCullFace(GL_BACK);

    glUseProgram(0);
}

void Widget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h); // 뷰포트 크기를 설정
}

void Widget::paintGL() {

    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glDisable(GL_CULL_FACE);     // 후면 컬링 비활성화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspectRatio = float(width()) / float(height());

    mat4 model = rotate(mat4(1.0f), radians(rotationAngle), vec3(0.0f, 1.0f, 0.0f));
    mat4 view = lookAt(vec3(0.0f, 5.0f, 3.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), aspectRatio, 0.1f, 100.0f);

    glUseProgram(shaderProgram);

    setUniforms(shaderProgram, model, view, projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glUseProgram(0);

}
