#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QDebug>
#include <QVector4D>

void Chapt1_3(){
    QMatrix4x4 view;
    view.lookAt(QVector3D(0.0f, 0.0f, 5.0f),
                QVector3D(0.0f, 0.0f, 0.0f),
                QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 model;
    model.rotate(90.0f, 0.0f, 1.0f, 0.0f);

    QMatrix4x4 mv = view * model;

    QVector4D position(1.0f, 0.0f, 0.0f, 1.0f);
    QVector4D transformed = mv*position;

    qDebug() << "변환된 좌표 : " << transformed;
}
