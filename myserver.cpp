#include "myserver.h"

MyServer::MyServer(QObject *parent):
    QTcpServer(parent)
{

}

MyServer::~MyServer()
{

}

void MyServer::incomingConnection(qintptr socketDescriptor)
{
    bool isOk = false;
    //检查是否有未运行的线程
    for(int thread_idx = 0; thread_idx < m_threads.size(); thread_idx++){
        if(!m_threads.at(thread_idx).isRunning()){

            isOk = true;
        }
    }
}
