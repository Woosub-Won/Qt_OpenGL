#include "qt_opengl_window_example_main.h"

#include "qt_opengl_window_example_trianglewindow.h"

#include <QGuiApplication>
#include <QSurfaceFormat>

int qt_opengl_window_example(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    TriangleWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
