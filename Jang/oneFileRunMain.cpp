#include <QGuiApplication>
#include <QOpenGLWindow>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <vector>
#include <iostream>

class TriangleWindow : public QOpenGLWindow, protected QOpenGLFunctions_4_0_Core
{
public:
    TriangleWindow() : m_program(0) {}

protected:
    void initializeGL() override {
        // OpenGL 함수 초기화
        initializeOpenGLFunctions();

        // 셰이더 생성 및 프로그램 빌드
        createShaderProgram();

        // 삼각형 데이터 초기화
        initTriangleData();
    }

    void resizeGL(int w, int h) override {
        // 뷰포트 설정
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        // 화면 초기화
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 셰이더 프로그램 활성화
        glUseProgram(m_program);

        // VAO 바인딩 및 삼각형 렌더링
        m_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        m_vao.release();
    }

private:
    GLuint m_program; // 셰이더 프로그램 ID
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_cbo;

    void createShaderProgram() {
        // 정점 셰이더 코드
        const char *vertexShaderSource = R"(
            #version 400
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec3 color;
            out vec3 fragColor;
            void main() {
                fragColor = color;
                gl_Position = vec4(position, 1.0);
            }
        )";

        // 프래그먼트 셰이더 코드
        const char *fragmentShaderSource = R"(
            #version 400
            in vec3 fragColor;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(fragColor, 1.0);
            }
        )";

        // 정점 셰이더 생성 및 컴파일
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        // 프래그먼트 셰이더 생성 및 컴파일
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        // 셰이더 프로그램 생성 및 링크
        m_program = glCreateProgram();
        glAttachShader(m_program, vertexShader);
        glAttachShader(m_program, fragmentShader);
        glLinkProgram(m_program);
        checkProgramLinkStatus(m_program);

        // 셰이더 객체 삭제
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void initTriangleData() {
        // 정점 데이터
        std::vector<GLfloat> vertices = {
            0.0f,  0.5f, 0.0f,  // 위쪽 꼭짓점
            -0.5f, -0.5f, 0.0f,  // 왼쪽 아래
            0.5f, -0.5f, 0.0f   // 오른쪽 아래
        };

        // 색상 데이터
        std::vector<GLfloat> colors = {
            1.0f, 0.0f, 0.0f,  // 빨강
            0.0f, 1.0f, 0.0f,  // 초록
            0.0f, 0.0f, 1.0f   // 파랑
        };

        // VAO 생성 및 바인딩
        m_vao.create();
        m_vao.bind();

        // VBO 생성 및 데이터 전송
        m_vbo.create();
        m_vbo.bind();
        m_vbo.allocate(vertices.data(), vertices.size() * sizeof(GLfloat));

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // CBO 생성 및 데이터 전송
        m_cbo.create();
        m_cbo.bind();
        m_cbo.allocate(colors.data(), colors.size() * sizeof(GLfloat));

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        m_vao.release();
    }

    void checkShaderCompileStatus(GLuint shader) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader Compilation Failed\n" << infoLog << std::endl;
        }
    }

    void checkProgramLinkStatus(GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "ERROR: Program Linking Failed\n" << infoLog << std::endl;
        }
    }
};

int main_oneFile(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 0); // OpenGL 4.0 Core Profile
    format.setProfile(QSurfaceFormat::CoreProfile);

    TriangleWindow window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}
