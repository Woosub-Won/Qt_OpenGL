#include <QWidget>
#include <QSurfaceFormat>
#include <QOpenGLWidget>

int Chapt1_1()
{

    QSurfaceFormat format;
    format.setVersion(4,0);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QOpenGLWidget* widget = new QOpenGLWidget();
    widget->setFormat(format);
    widget->show();

    return 0;
}
