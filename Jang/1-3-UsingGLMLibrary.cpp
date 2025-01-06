#include <QMatrix4x4>
#include <QSurfaceFormat>
#include <QDebug>
#include <QCoreApplication>
#include <QOpenGLWidget>


int one_three_GLM(int argc, char *argv[]) {
    // QCoreApplication app(argc, argv);

    // glew 안쓰고 해볼 예정

    qDebug() << "1-3";

    QVector4D position = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
    QMatrix4x4 view;
    view.lookAt(QVector3D(0.0, 0.0, 5.0),
                QVector3D(0.0, 0.0, 0.0),
                QVector3D(0.0, 1.0, 0.0));
    // QMatrix4x4 model = QMatrix4x4(1.0f);
    QMatrix4x4 model; //서로 다른 방식의 대각행렬 생성
    model.setToIdentity();
    model.rotate(90.0f, QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 mv = view * model;
    QVector4D transformed = mv * position;

    qDebug() << transformed;

    return 0;
    // return app.exec();
}
