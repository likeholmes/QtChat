#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QPixmap>
#include<QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->label->setText(QDir("../Server/resource/Avatar001.jpg").absolutePath());
    ui->label->setPixmap(QPixmap(QDir("../Server/resource/Avatar001.jpg").absolutePath()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    QPixmap test(*ui->label->pixmap());
    test.save(QDir("../Server/resource/1.jpg").absolutePath());
    ui->label->setText("saved!!!");
}
