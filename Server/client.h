#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFileInfo>
#include "user.h"
#include "request.h"
#include "response.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    Client(QTcpSocket *socket, QObject *parent = nullptr);

    QSharedPointer<Request> nextPendingRequest();

    void sendMsg();

    void receiveMsg();

    void login();

    void doRegister();

    void search();

    void addFriend();

    void download(int index);

    QString account() const;

    void disconnect();

signals:

    void disconnected();

    void newRequest();

    void error(const QString& err);

private:
    void upload(Message &msg);

    int getId(const QString& account);

    QString filePath(int fileIndex);

    QString getAccount(int id);

    Request m_request;
    QTcpSocket *m_socket;
    QString m_token;
    User m_user;
};

#endif // CLIENT_H
