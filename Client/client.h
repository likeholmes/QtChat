#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "user.h"
#include "message.h"
#include <QQmlListProperty>
#include <QDataStream>

class Client: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<User> searchContent READ searchContent);

    Q_PROPERTY(User* user READ user);

public:
    Client(QObject *parent = nullptr);

    ~Client();

    void dealError(QAbstractSocket::SocketError error);

    QQmlListProperty<User> searchContent();
    void appendUser(User* user);
    int userCount() const;
    User *searchUser(int idx) const;
    void clearUsers();

    User* user() const{
        return m_user;
    }

    Q_INVOKABLE void dealLogin(const QString &account, const QString & password);

    Q_INVOKABLE void dealRegister(User* user);

    Q_INVOKABLE void dealSearch(const QString &searchContent);

    Q_INVOKABLE void dealAddFriend(const QString &account);

    Q_INVOKABLE void dealSendMsg(Message* message);

    Q_INVOKABLE void dealDownload(const QString &path);

    void responseHandle();

    void dealData();

    void dealAccept();

signals:

    void loginSuccess();

    void loginFail();

    void registerSuccess();

    void registerFail();

    void searchSuccess();

    void searchFail();

    void addSuccess();

    void addFail();

    void msgReceived();

    void sendSuccess();

    void sendFail();

    void downloadSuccess();

    void dataCome();

    void acceptSuccess();

private:

    void writeSocket(const QByteArray &bytes);

    static void appendUser(QQmlListProperty<User> *, User *);
    static int userCount(QQmlListProperty<User> *);
    static User *searchUser(QQmlListProperty<User> *, int);
    static void clearUsers(QQmlListProperty<User> *);

    void upload(const QString &path);

    void download(const Message &msg);

    QTcpSocket *socket;
    QString m_token;
    QList<User *> m_searchContent;
    qint64 chunk;
    //QList<Message *>m_msg;
    User *m_user;
    QString m_downloadPath;
    QList<QString> typeMap;
    QByteArray m_data;
    QDataStream in;
};

#endif // CLIENT_H
