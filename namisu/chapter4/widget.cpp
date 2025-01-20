#include "widget.h"
#include "sphere_data.h"
#include "cube_data.h"
#include "cube_data_no_top.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <QTimer> //Qt 타이머 추가
#include <QLabel>
#include <QVBoxLayout>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent), shaderProgram(0), VAO(0), VBO(0), rotationAngle(0.0f) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]() {
        rotationAngle += 1.0f;
        if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
        update();
    });
    //timer->start(16);
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

    glUniform1i(glGetUniformLocation(shaderProgram, "DrawSkyBox"), true);
    glUniform3f(glGetUniformLocation(shaderProgram, "WorldCameraPosition"), eye.x, eye.y, eye.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelMatrix"), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ModelViewMatrix"), 1, GL_FALSE, value_ptr(view * model));
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "NormalMatrix"), 1, GL_FALSE, value_ptr(normalMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjectionMatrix"), 1, GL_FALSE, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, value_ptr(mvp));

    glUniform1f(glGetUniformLocation(shaderProgram, "ReflectFactor"), 0.85);
    glUniform4f(glGetUniformLocation(shaderProgram, "MaterialColor"), 1.0, 0.5, 0.3, 0.1);
}

void Widget::initializeBuffers() {
    FILE* file = NULL;
    fopen_s(&file, "C:/obj/cube.obj", "r");
    ReadOBJ(file);

    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * faceCount * 3, vertexData, GL_STATIC_DRAW);

    GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));

    GLuint vNormal = glGetAttribLocation(shaderProgram, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

    GLuint vTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) + sizeof(vec3)));

    GLuint vTangent = glGetAttribLocation(shaderProgram, "vTangent");
    glEnableVertexAttribArray(vTangent);
    glVertexAttribPointer(vTangent, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) + sizeof(vec3) + sizeof(vec2)));

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

}




void setTwoSidedUniform(GLuint shaderProgram){
    glUniform4f(glGetUniformLocation(shaderProgram, "Light.Position"), 0.0f, 0.0f, 10.0f, 1.0f); // 조명 위치
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Intensity"), 0.5f, 0.5f, 0.5f);       // 조명 강도

    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ka"), 1.0f, 1.0f, 1.0f);           // Ambient 반사율
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ks"), 0.5f, 0.5f, 0.5f);           // Specular 반사율
    glUniform1f(glGetUniformLocation(shaderProgram, "Material.Shininess"), 32.0f);               // 하이라이트
}

void setLightUniform(GLuint shaderProgram){
    glUniform4f(glGetUniformLocation(shaderProgram, "Light.Position"), 0.0f, 0.0f, 2.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.La"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Ld"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Light.Ls"), 0.5f, 0.5f, 0.5f);

    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ka"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Kd"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Material.Ks"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "Material.Shininess"), 50.0f);
}


void setFlatUniform(GLuint shaderProgram){
    glUniform3f(glGetUniformLocation(shaderProgram, "LightPosition"), 0.0f, 0.0f, 2.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "LightIntensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Ka"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(shaderProgram, "Kd"), 0.8f, 0.5f, 0.3f);
}

void Widget::initializeSphereBuffers(const vector<float>& sphereVertices) {
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

void Widget::loadTexture(const char* filename, int idx) {
    QImage timg = QImage(filename).convertToFormat(QImage::Format_RGBA8888);

    // 텍스처 생성
    switch(idx){
        case 0: glActiveTexture(GL_TEXTURE0); break;
        case 1: glActiveTexture(GL_TEXTURE1); break;
        case 2: glActiveTexture(GL_TEXTURE2); break;
    }

    glGenTextures(1, &tid[idx]);
    glBindTexture(GL_TEXTURE_2D, tid[idx]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, timg.width(), timg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, timg.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void Widget::setCubeMap(){
    glActiveTexture(GL_TEXTURE0);
    GLuint texID;
    glGenTextures(1, &tid[0]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid[0]);
    const char * suffixes[] = { "posx", "negx", "posy",
                              "negy", "posz", "negz" };
    GLuint targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for( int i = 0; i < 6; i++ ) {
        string texName = string("C:/ImageTemp/cube/pisa_") + suffixes[i] + ".png";
        QImage timg = QImage(texName.c_str()).convertToFormat(QImage::Format_RGBA8888);
        glTexImage2D(targets[i], 0, GL_RGBA, timg.width(), timg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, timg.bits());
    }

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                    GL_CLAMP_TO_EDGE);

    GLuint uniloc = glGetUniformLocation(shaderProgram, "CubeMapTex");
    glUniform1i(uniloc, 0);
}


void Widget::initializeGL() {
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "GLEW initialized. OpenGL version: " << glGetString(GL_VERSION) << endl;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 밝은 회색

    shaderProgram = InitShader( "C:/Users/SeunghyunWoo/Documents/chapter4/vshader_ch5.vert",
                               "C:/Users/SeunghyunWoo/Documents/chapter4/fshader_ch5.frag");

    glUseProgram(shaderProgram);
    initializeBuffers();

    setTwoSidedUniform(shaderProgram);
    //setLightUniform(shaderProgram);
    setCubeMap();

    //loadTexture("C:/ImageTemp/ogre_diffuse.png", 0);
    //loadTexture("C:/ImageTemp/ogre_normalmap.png", 1);
    //loadTexture("C:/ImageTemp/ao_ears.png", 2);

    updateCameraPosition();
}

void Widget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h); // 뷰포트 크기를 설정
}

void Widget::paintGL() {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspectRatio = float(width()) / float(height());

    mat4 projection = perspective(radians(45.0f), float(width()) / height(), 0.1f, 100.0f);
    mat4 view = lookAt(eye, at, up);
    mat4 model = mat4(1.0f);
    model = scale(model, vec3(50.0f, 50.0f, 50.0f));

    glUseProgram(shaderProgram);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, tid[0]); // tid는 loadTexture에서 생성한 텍스처 ID
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, tid[1]); // tid는 loadTexture에서 생성한 텍스처 ID
    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, tid[2]); // tid는 loadTexture에서 생성한 텍스처 ID

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid[0]); // 스카이박스 텍스처 바인딩

    setCubeMap();
    glUniform1i(glGetUniformLocation(shaderProgram, "CubeMapTex"), 0); // CubeMap 텍스처 설정

    //glUniform1i(glGetUniformLocation(shaderProgram, "ColorMapTex"), 0);
    //glUniform1i(glGetUniformLocation(shaderProgram, "NormalMapTex"), 1);
    //glUniform1i(glGetUniformLocation(shaderProgram, "DiffuseMapTex"), 2);

    setUniforms(shaderProgram, model, view, projection);

    glUniform1i(glGetUniformLocation(shaderProgram, "DrawSkyBox"), true);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, faceCount * 3);

}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = event->pos() - prevMousePos;
    prevMousePos = event->pos();

    if (event->buttons() & Qt::LeftButton) {
        yaw -= delta.x() * sensitivity;
        pitch += delta.y() * sensitivity;

        updateCameraPosition();
    } else if (event->buttons() & Qt::RightButton) {
        radius -= delta.y() * 0.05f;
        //radius = glm::clamp(radius, 1.0f, 25.0f);

        updateCameraPosition();
    }
}

void Widget::mousePressEvent(QMouseEvent *event) {
    prevMousePos = event->pos();
}

void Widget::updateCameraPosition() {
    // 구면 좌표계 기반 카메라 위치 계산
    eye.x = at.x + radius * cos(pitch) * sin(yaw);
    eye.y = at.y + radius * sin(pitch);
    eye.z = at.z + radius * cos(pitch) * cos(yaw);

    up.y = cos(pitch);
    update();
}


void Widget::ReadOBJ(FILE* file) {
    char lineHeader[128];

    // 데이터 저장을 위한 벡터
    vector<vec4> vertices;      // 정점 좌표
    vector<vec2> texCoords;    // 텍스처 좌표
    vector<vec3> normals;      // 법선 벡터
    vector<ivec3> faces;       // 면 데이터 (정점/텍스처/법선 인덱스)

    float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
    float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "v ", 2) == 0) {
            // 정점 데이터 처리
            vec4 vertex;
            sscanf(buffer + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertex.w = 1.0f;

            // 최소값/최대값 갱신
            if (vertex.x < minX) minX = vertex.x;
            if (vertex.y < minY) minY = vertex.y;
            if (vertex.z < minZ) minZ = vertex.z;
            if (vertex.x > maxX) maxX = vertex.x;
            if (vertex.y > maxY) maxY = vertex.y;
            if (vertex.z > maxZ) maxZ = vertex.z;

            vertices.push_back(vertex);
        } else if (strncmp(buffer, "vt ", 3) == 0) {
            // 텍스처 좌표 처리
            vec2 texCoord;
            sscanf(buffer + 3, "%f %f", &texCoord.x, &texCoord.y);
            texCoords.push_back(texCoord);
        } else if (strncmp(buffer, "vn ", 3) == 0) {
            // 법선 벡터 처리
            vec3 normal;
            sscanf(buffer + 3, "%f %f %f", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        } else if (strncmp(buffer, "f ", 2) == 0) {
            // 면(Face) 데이터 처리
            ivec3 face[3];
            sscanf(buffer + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                   &face[0].x, &face[0].y, &face[0].z,
                   &face[1].x, &face[1].y, &face[1].z,
                   &face[2].x, &face[2].y, &face[2].z);

            faces.push_back(face[0]);
            faces.push_back(face[1]);
            faces.push_back(face[2]);
        }
    }

    fclose(file);

    // 모델 중심 및 크기 조정
    vec3 center = vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
    float maxDimension = max({maxX - minX, maxY - minY, maxZ - minZ});
    float scale = 1.0f / maxDimension;

    for (auto& vertex : vertices) {
        vertex.x = (vertex.x - center.x) * scale;
        vertex.y = (vertex.y - center.y) * scale;
        vertex.z = (vertex.z - center.z) * scale;
    }

    // 탱젠트 벡터 계산 및 데이터 저장
    vertexData = (Vertex*)malloc(sizeof(Vertex) * faces.size());
    std::vector<vec4> tangents(vertices.size(), vec4(0.0f));

    for (size_t i = 0; i < faces.size(); i += 3) {
        int idx1 = faces[i].x - 1;
        int idx2 = faces[i + 1].x - 1;
        int idx3 = faces[i + 2].x - 1;

        vec3 edge1 = vec3(vertices[idx2]) - vec3(vertices[idx1]);
        vec3 edge2 = vec3(vertices[idx3]) - vec3(vertices[idx1]);

        vec2 deltaUV1 = texCoords[faces[i + 1].y - 1] - texCoords[faces[i].y - 1];
        vec2 deltaUV2 = texCoords[faces[i + 2].y - 1] - texCoords[faces[i].y - 1];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);

        tangents[idx1] += vec4(normalize(tangent), 1.0f);
        tangents[idx2] += vec4(normalize(tangent), 1.0f);
        tangents[idx3] += vec4(normalize(tangent), 1.0f);
    }

    // 정점 데이터에 탱젠트 포함
    for (size_t i = 0; i < faces.size(); i++) {
        int idx = faces[i].x - 1;
        int texCoordIdx = faces[i].y - 1;
        int normalIdx = faces[i].z - 1;

        vertexData[i].position = vertices[idx];
        vertexData[i].texCoord = texCoords[texCoordIdx];
        vertexData[i].normal = normals[normalIdx];
        vertexData[i].tangent = tangents[idx];
    }

    faceCount = faces.size() / 3; // 삼각형 면 개수 계산
}

vec4 Widget::calculateSurfaceNormal(vec4 v1, vec4 v2, vec4 v3){
    vec3 v1v2 = vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
    vec3 v1v3 = vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
    vec3 normal = normalize(cross(v1v2, v1v3));
    return vec4(normal, 1.0);
}

