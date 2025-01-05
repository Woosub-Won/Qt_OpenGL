#include <QApplication>
#include <QSurfaceFormat>
#include "qt_opengl_window_example_main.h"
#include "chapter1.h"

int main(int argc, char *argv[])
{
    // QApplication a(argc, argv);

    qDebug() << "Qt version:" << QT_VERSION_STR;

    one_one_Introduction(argc, argv);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    qDebug() << format.profile();
    qDebug() << format.version();

    one_two_GLEW(argc, argv);
    one_three_GLM(argc, argv);
    one_four_version(argc, argv);
    one_five_shader(argc, argv);



    // qt_opengl_window_example(argc, argv);

    return 0;
    // return a.exec();
}
