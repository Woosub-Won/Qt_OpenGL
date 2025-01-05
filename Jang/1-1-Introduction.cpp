#include <QSurfaceFormat>
#include <QDebug>
//#include <QCoreApplication>
#include <QApplication>
#include <QOpenGLWidget>


int one_one_Introduction(int argc, char *argv[]) {
    // QCoreApplication app(argc, argv);

    qDebug() << "1-1";

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,0);

    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    QOpenGLWidget *widget = new QOpenGLWidget();
    // QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    widget->setFormat(format); // must be called before the widget or its parent window gets shown

    //widget->show();

    return 0;
    // return app.exec();
}
