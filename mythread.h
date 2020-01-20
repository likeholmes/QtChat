#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "mysocket.h"

#include <QThread>
#include <QList>
#include <QJsonObject>

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
    void handleRequest(QByteArray &data, MySocket *socket);
    void dealRegister(QJsonObject &json, MySocket *socket);
    void dealLogin(QJsonObject &json, MySocket *socket);
    void dealSearch(QJsonObject &json, MySocket *socket);
    void dealSend(QJsonObject &json, MySocket *socket);
    QString encrypt(QString password); //密码加密
    QByteArray generateResponse(QString action, QString response, QString type = "",
                                QString sender = "", QString receiver = "",
                                QByteArray *content = nullptr, QString format = "");
    QString generateToken(int id);

    QList<MySocket *> m_sockets;
};

#endif // MYTHREAD_H
