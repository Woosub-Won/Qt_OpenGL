#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <GL/freeglut.h>


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    QApplication a(argc, argv);
    mainwindow w;
    w.show();

    return a.exec();
}
