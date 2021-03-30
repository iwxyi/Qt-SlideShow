#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->addImage(QPixmap(":/test/images/img1.jpg"));
    ui->widget->addImage(QPixmap(":/test/images/img2.jpg"));
    ui->widget->addImage(QPixmap(":/test/images/img3.jpg"));
    ui->widget->addImage(QPixmap(":/test/images/img4.jpg"));
    ui->widget->addImage(QPixmap(":/test/images/img5.jpg"));
    ui->widget->setCurrentIndex(0);
    ui->widget->adjustSize();
    ui->widget->setAutoSlide(5000);

    connect(ui->widget, &SlideShow::signalImageClicked, this, [=](int index){
        qDebug() << "单击：" << index;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

