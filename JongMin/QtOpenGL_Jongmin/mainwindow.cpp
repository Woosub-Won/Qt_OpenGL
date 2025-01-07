#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtOpenGLWidgets/QOpenGLWidget>
#include "ch1_7_sendingdatatoshader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("OpenGL_Qt_Cmake");

    Ch1_7_SendingDataToShader* ch1_7Widget = new Ch1_7_SendingDataToShader(this);

    ui->verticalLayout->addWidget(ch1_7Widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
