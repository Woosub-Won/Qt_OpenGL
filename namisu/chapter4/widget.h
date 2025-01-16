#ifndef WIDGET_H
#define WIDGET_H

#include <C:/glm-master/glm/glm.hpp>
#include <C:/glm-master/glm/gtc/matrix_transform.hpp>
#include <C:/glm-master/glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <QOpenGLWidget>

using namespace glm;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    GLuint shaderProgram; // 셰이더 프로그램 ID
    GLuint VAO, VBO;      // VAO와 VBO
    float rotationAngle = 0.0f;
    QTimer *timer;             // 애니메이션 타이머
    GLuint loadShader(const char* filePath, GLenum shaderType); // 셰이더 로드 함수
    GLuint InitShader(const char* vertexPath, const char* fragmentPath);

    int drawSize = 0;
    void setUniforms(GLuint program, const mat4& model, const mat4& view, const mat4& projection);
    void initializeCubeBuffers();
    void initializeSphereBuffers(const std::vector<float>& sphereVertices);
    void loadTexture(const char* filename);
};

#endif // WIDGET_H
