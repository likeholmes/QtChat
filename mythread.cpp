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

}

int MyThread::getSocketCount()
{
    return m_sockets.size();
}

void MyThread::addSocket(qintptr socketDescriptor)
{
    QTcpSocket socket;
    if(!socket.setSocketDescriptor(socketDescriptor)){
        //抛出错误
    }
    //m_sockets.push_front()
}
