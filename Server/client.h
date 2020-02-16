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

    void nextPendingRequest();

    void sendMsg();

    void receiveMsg();

    void login();

    void doRegister();

    void search();

    void addFriend();

    void accept();

    void download(int index);

    QString account() const{
        return m_user.account();
    }

    Request request() const{
        return m_request;
    }

    void disconnect();

signals:

    void disconnected();

    void newRequest();

    void error(const QString& err);

    void send(const QString& account);

    void add(const QString& account);

private:
    void upload(Message &msg);

    void writeSocket(const QByteArray &bytes);

    int getId(const QString& account);

    Message::Type getType(int fileIndex);

    QString getPath(int fileIndex);

    QString getAccount(int id);

    int getFileId(int fileIndex);

    Request m_request;
    QTcpSocket *m_socket;
    QString m_token;
    User m_user;
    QByteArray end;
};

#endif // CLIENT_H
