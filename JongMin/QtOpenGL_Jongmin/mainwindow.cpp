#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtOpenGLWidgets/QOpenGLWidget>
#include "ch1_7_sendingdatatoshader.h"
#include "ch1_8_gettingalist.h"
#include "ch1_9_sendingdatatoashader.h"
#include "ch1_10_usinguniformblocks.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("OpenGL_Qt_Cmake");

    //Ch1_7_SendingDataToShader* ch1_7Widget = new Ch1_7_SendingDataToShader(this);
    //ch1_8_GettingAList* ch1_8Widget = new ch1_8_GettingAList(this);
    //ch1_9_SendingDataToAShader* ch1_9Widget = new ch1_9_SendingDataToAShader(this);
    ch1_10_UsingUniformBlocks* ch1_10Widget = new ch1_10_UsingUniformBlocks(this);
    ui->verticalLayout->addWidget(ch1_10Widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
