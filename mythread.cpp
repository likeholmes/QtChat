#include "mythread.h"

MyThread::MyThread(QObject *parent):
    QThread(parent)
{

}

MyThread::~MyThread()
{

}

void MyThread::run()
{
    for (auto socket: m_sockets){
        if(socket->bytesAvailable() > 0) //若套接字内有可读取的数据
        {
            //从套接字中取出数据进行处理

        }
    }
}

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
