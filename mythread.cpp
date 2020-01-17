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
