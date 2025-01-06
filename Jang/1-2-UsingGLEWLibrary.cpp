#include <QMatrix4x4>
#include <QSurfaceFormat>
#include <QDebug>
#include <QCoreApplication>
#include <QOpenGLWidget>


int one_two_GLEW(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // glew 안쓰고 해볼 예정

    qDebug() << "1-2";
    qDebug() << "glew not using";

    // GLenum err = glewInit();
    // if( GLEW_OK != err )
    // {
    //     fprintf(stderr, "Error initializing GLEW: %s\n",
    //             glewGetErrorString(err) );
    // }

    return 0;
    // return app.exec();
}
