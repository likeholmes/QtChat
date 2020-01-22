#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setReadOnly(true);
    //connect(&server, &MyServer::newConnection, this, )
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_pushButton_clicked()
{
    log("打开服务器");
    server.listen(QHostAddress::Any, 1031);
}

void MainWindow::on_close_pushButton_clicked()
{
    log("关闭服务器");
    server.close();
}

void MainWindow::log(QString info)
{
    ui->textBrowser->append(info+"\n");
}
