#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto generalPixmap = [=](int index = 0) -> QPixmap{
        QPixmap pixmap(400, 100);
        pixmap.fill(QColor::fromHsl(qrand() % 360, qrand() % 256, qrand() % 200));
        return pixmap;
    };

//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
//    ui->widget->addPixmap(generalPixmap());
    ui->widget->addPixmap(QPixmap(":/test/images/img1.jpg"));
    ui->widget->addPixmap(QPixmap(":/test/images/img2.jpg"));
    ui->widget->addPixmap(QPixmap(":/test/images/img3.jpg"));
    ui->widget->addPixmap(QPixmap(":/test/images/img4.jpg"));
    ui->widget->addPixmap(QPixmap(":/test/images/img5.jpg"));
    ui->widget->setCurrentIndex(0);
    ui->widget->adjustSize();

    connect(ui->widget, &SlideShow::signalCardClicked, this, [=](int index){
        qDebug() << "单击：" << index;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

