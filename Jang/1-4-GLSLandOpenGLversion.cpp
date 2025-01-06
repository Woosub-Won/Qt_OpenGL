#include <QMatrix4x4>
#include <QSurfaceFormat>
#include <QDebug>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QEventLoop>

int one_four_version(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << "1-4";

    // widget 같은걸 써야 opengl 연결 시작해서 있어야 돌아감
    QOpenGLWidget *widget = new QOpenGLWidget();

    widget->show();

    QOpenGLFunctions check;

    check.initializeOpenGLFunctions();

    const GLubyte *renderer = check.glGetString(GL_RENDERER);
    const GLubyte *vendor = check.glGetString(GL_VENDOR);
    const GLubyte *version = check.glGetString( GL_VERSION );
    const GLubyte *glslVersion =check.glGetString( GL_SHADING_LANGUAGE_VERSION );

    GLint major, minor;
    check.glGetIntegerv(GL_MAJOR_VERSION, &major);
    check.glGetIntegerv(GL_MINOR_VERSION, &minor);

    qDebug() << "GL Vendor:" << reinterpret_cast<const char*>(vendor);
    qDebug() << "GL Renderer:" << reinterpret_cast<const char*>(renderer);
    qDebug() << "GL Version (string):" << reinterpret_cast<const char*>(version);
    qDebug() << "GL Version (integer):" << major << "." << minor;
    qDebug() << "GLSL Version:" << reinterpret_cast<const char*>(glslVersion);
    qDebug() << "";

    // printf("GL Vendor : %s\n", vendor);
    // printf("GL Renderer : %s\n", renderer);
    // printf("GL Version (string) : %s\n", version);
    // printf("GL Version (integer) : %d.%d\n", major, minor);
    // printf("GLSL Version : %s\n", glslVersion);
    // qDebug() << "";

    return 0;
}
