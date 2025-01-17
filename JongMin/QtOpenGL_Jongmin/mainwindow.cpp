#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtOpenGLWidgets/QOpenGLWidget>
#include "ch1_7_sendingdatatoshader.h"
#include "ch1_8_gettingalist.h"
#include "ch1_9_sendingdatatoashader.h"
#include "ch1_10_usinguniformblocks.h"
#include "ch2_3implementingpervertex.h"
#include "ch3_7_creatingacartoonshading.h"
#include "ch4_2_applyinga2dtexture.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("OpenGL_Qt_Cmake");

    //Ch1_7_SendingDataToShader* ch1_7Widget = new Ch1_7_SendingDataToShader(this);
    //ch1_8_GettingAList* ch1_8Widget = new ch1_8_GettingAList(this);
    //ch1_9_SendingDataToAShader* ch1_9Widget = new ch1_9_SendingDataToAShader(this);
    //ch1_10_UsingUniformBlocks* ch1_10Widget = new ch1_10_UsingUniformBlocks(this);
    //ch2_3ImplementingPerVertex* ch2_3Widget = new ch2_3ImplementingPerVertex(this);
    ch3_7_CreatingACartoonShading* ch3_7Widget = new ch3_7_CreatingACartoonShading(this);
    ch4_2_ApplyingA2DTexture* ch4_2Widget = new ch4_2_ApplyingA2DTexture(this);
    ui->verticalLayout_2->addWidget(ch3_7Widget);
    ui->verticalLayout->addWidget(ch4_2Widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
