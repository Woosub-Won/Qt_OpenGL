#include <QApplication>
#include <QWidget>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include "Chapt1.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // QApplication 객체 생성
    //Chapt1_1();
     // Chapt1_2();
    // Chapt1_3();
    // Chapt1_4();
    qDebug() << "실행";
    return app.exec();             // 이벤트 루프 실행
}
