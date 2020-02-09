#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "request.h"

#include<QSharedPointer>
#include<QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setReadOnly(true);
    m_server = new QTcpServer();
    connect(m_server, &QTcpServer::newConnection, this, &MainWindow::addClient);
}

MainWindow::~MainWindow()
{
    delete m_server;
    delete ui;
}

void MainWindow::on_start_pushButton_clicked()
{
    log("打开服务器");
    m_server->listen(QHostAddress::Any, 1031);
}

void MainWindow::on_close_pushButton_clicked()
{
    log("关闭服务器");
    m_server->close();
}

void MainWindow::log(QString info)
{
    ui->textBrowser->append(info);
}

//信号响应
void MainWindow::handleRequest()
{
    log("捕捉请求，将发送者转换为客户端");
    Client *client = (Client *)sender();
    log("获取从客户端传来的请求");
    //用指针怕不知道何时释放空间，用值传递太浪费时间了
    Request res = client->request();
    log("检查请求类别");
    if(res.isNull()){
        log("空请求");
        return;
    }
    log("新请求");
    switch(res.action())
    {
    case Request::Login:
        log("请求登录");
        client->login();
        log(client->account() + "登录成功!");
        break;
    case Request::Register:
        log("请求注册");
        client->doRegister();
        break;
    case Request::Search:
        log(client->account() + "请求搜索");
        client->search();
        break;
    case Request::Add:
        log(client->account() + "请求添加");
        client->addFriend();
        break;
    case Request::Download:
        log(client->account() + "请求下载");
        client->download(res.downloadIndex());
        break;
    case Request::Send:
        log(client->account() + "请求发送");
        //某客户端发送消息给另一个用户，检查用户是否在客户端列表中（若客户端很多，需用map或hash）
        //不能用map或hash，同时不在线的人数可能非常多，如果事先分配好，将造成空间的浪费
        //可以用hash或map记录是否在线，用空间换时间
        //若存在则直接将msg发送给该用户的客户端
        //若不存在，则将消息存储在（临时数据库中？）等客户端上线时再发送
        client->sendMsg(); //需要表明文件标识，使其它客户端可以通过其下载和传输数据，还要编辑响应传回客户端
        //将聊天信息存储到数据库中
        //检查是否还有其它数据要上传
        //若有文件上传则将其上传至服务器，并将文件信息存储到数据库中
        break;
    }
}

//信号响应
void MainWindow::addClient()
{
    log("新连接");
    //能实现吗
    Client *client = new Client(m_server->nextPendingConnection());
    //能实现吗，在别的类中？
    connect(client, &Client::disconnected ,this, &MainWindow::removeClient);
    connect(client, &Client::newRequest, this, &MainWindow::handleRequest);
    connect(client, &Client::error, this, &MainWindow::dealError);
    connect(client, &Client::add, this, &MainWindow::dealAdd);
    connect(client, &Client::send, this, &MainWindow::dealSend);
    //需加锁
    m_clients << client;
    //需加锁
    log("新连接");
}

//信号响应
void MainWindow::removeClient()
{
    //找到对应的sender套接字，释放空间，并从列表中移除

    //应该保护，需要加锁
    for(int i = 0; i < m_clients.size(); ++i){
        if(m_clients.at(i) == (Client *)sender()){ //指针比较我觉得不OK
            delete m_clients.at(i);
            m_clients.removeAt(i);
            break;
        }
    }
    //需加锁
    log("断开连接");
}

Client *MainWindow::findClient(const QString &account)
{
    for (int i = 0; i < m_clients.size(); ++i) {
        if(m_clients.at(i)->account() == account)
            return m_clients.at(i);
    }
    return nullptr;
}

void MainWindow::dealError(const QString &err)
{
    log(err);
}


void MainWindow::on_clear_pushButton_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::dealAdd(const QString &account)
{
    Client * oclient = findClient(account);
    if(oclient){
        oclient->accept();
    }
}

void MainWindow::dealSend(const QString &account)
{
    Client * oclient = findClient(account);
    if(oclient){
        oclient->receiveMsg();
    }
}
