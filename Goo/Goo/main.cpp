#include <QApplication>
#include <QWidget>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include "Chapt1.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // QApplication 객체 생성
    // Chapt1_1();
    // Chapt1_2();
    // Chapt1_3();
    // Chapt1_4();
    // Chapt1_5();
    // Chapt1_6();

    Chapt1_7();
    qDebug() << "running";
    return app.exec();             // 이벤트 루프 실행
}
