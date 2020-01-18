#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QList>
#include <QTcpSocket>

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
private:
    QList<QTcpSocket *> m_sockets;
};

#endif // MYTHREAD_H
