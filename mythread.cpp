#include "mythread.h"

#include <QDataStream>
#include <QJsonDocument>


MyThread::MyThread(QObject *parent):
    QThread(parent)
{

}

MyThread::~MyThread()
{

}

// 【注意：处理套接字的任何时候，套接字都有可能从客户端关闭】
void MyThread::run()
{
    //qint64 maxlen = 131072;
    for (auto socket: m_sockets){
        auto t_socket = socket;
        m_sockets.removeOne(socket);
        if(t_socket->bytesAvailable() > 0) //若套接字内有可读取的数据
        {
            //从套接字中取出数据进行处理
            QByteArray bytes;
            QDataStream in(t_socket);
            in.startTransaction();
            //一次读取完整数据
            in >> bytes;
            if(!in.commitTransaction())
            {
                //不处理
                return;
            }
            handleRequest(bytes, t_socket);
            m_sockets.push_back(t_socket); //处理完请求后将套接字加入队尾
            return;
        }
    }
}
// 【注意：处理套接字的任何时候，套接字都有可能从客户端关闭】

int MyThread::getSocketCount()
{
    return m_sockets.size();
}

void MyThread::addSocket(qintptr socketDescriptor)
{
    MySocket *socket = new MySocket();
    if(!socket->setSocketDescriptor(socketDescriptor)){
        //抛出错误
        emit error(socket->error());
        return;
    }
    m_sockets.push_front(socket);
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(removeSocket(socket)));
}

// 【线程什么时候会使用这个槽】
void MyThread::removeSocket(MySocket *socket)
{
    //是否要判断错误,以及删除顺序是否有误？
    delete socket;
    m_sockets.removeOne(socket);

}
// 【线程什么时候会使用这个槽】

void MyThread::handleRequest(QByteArray &data, MySocket *socket)
{
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();
    if(json.contains("action") && json["action"].isString())
    {
        QString action = json["action"].toString();
        if(action == "register"){ //注册
            dealRegister(json, socket);
        }else if(action == "login"){ //登录
            dealRegister(json, socket);
        }else if(action == "lookup"){ //查找

        }else if(action == "sendMsg"){
            dealSend(json, socket);
        }
    }
}
