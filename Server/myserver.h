#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QList>

#include "mythread.h"

class MyServer: public QTcpServer
{
    Q_OBJECT
public:
    MyServer(QObject *parent = nullptr);
    ~MyServer();

protected:
    void incomingConnection(qintptr handle) override;

private:
    QList<MyThread *> m_threads;
};

#endif // MYSERVER_H
