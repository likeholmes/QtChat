#include "myserver.h"

MyServer::MyServer(QObject *parent):
    QTcpServer(parent)
{

}

MyServer::~MyServer()
{

}

// 【问题九、十】
void MyServer::incomingConnection(qintptr socketDescriptor)
{
    //检查是否有未运行的线程
    for(int thread_idx = 0; thread_idx < m_threads.size(); thread_idx++){
        if(!m_threads.at(thread_idx)->isRunning()){
            m_threads.at(thread_idx)->addSocket(socketDescriptor);
            return;
        }
    }
    //检查当前线程内队列最短的线程
    int idx;
    int length = 100;
    for(int thread_idx = 0; thread_idx < m_threads.size(); thread_idx++){
        if(m_threads.at(thread_idx)->getSocketCount() < length){
            length = m_threads.at(thread_idx)->getSocketCount();
            idx = thread_idx;
        }
    }
    m_threads.at(idx)->addSocket(socketDescriptor);
}
// 【问题九、十】

