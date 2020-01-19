#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "mysocket.h"

#include <QThread>
#include <QList>

class MyThread: public QThread
{
    Q_OBJECT
public:
    MyThread(QObject *parent = nullptr);
    ~MyThread();
    int getSocketCount();
    void addSocket(qintptr socketDescriptor);
protected:
    void run() override;
signals:
    void listIsEmpty();
    void error(MySocket::SocketError error);
private slots:
    void removeSocket(MySocket *socket);
private:
    QList<MySocket *> m_sockets;
};

#endif // MYTHREAD_H
