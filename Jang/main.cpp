#include <QCoreApplication>
#include <QSurfaceFormat>
#include "qt_opengl_window_example_main.h"
#include "chapter1.h"

int main(int argc, char *argv[])
{
    // QCoreApplication a(argc, argv);

    one_one_Introduction(argc, argv);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    qDebug() << format.profile();
    qDebug() << format.version();

    one_two_GLEW(argc, argv);
    one_three_GLM(argc, argv);
    one_four_version(argc, argv);




    // qt_opengl_window_example(argc, argv);

    // return a.exec();
}
