#include <QSurfaceFormat>
#include <QDebug>
#include <QCoreApplication>


int one_one_Introduction(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);

    qDebug() << format.profile();

    format.setVersion(4,0);

    qDebug() << format.version();

    return app.exec();
}
