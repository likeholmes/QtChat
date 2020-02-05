#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "user.h"
#include "message.h"
#include <QQmlListProperty>

class Client: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<User> searchContent READ searchContent);

    Q_PROPERTY(Message* message READ message);

    Q_PROPERTY(QString downloadPath READ downloadPath);

public:
    Client(QObject *parent = nullptr);

    ~Client();

    void dealError(QAbstractSocket::SocketError error);

    QQmlListProperty<User> searchContent();
    void appendUser(User* user);
    int userCount() const;
    User *searchUser(int idx) const;
    void clearUsers();

    Message* message() const;

    QString downloadPath() const;

    Q_INVOKABLE void dealLogin(const QString &account, const QString & password);

    Q_INVOKABLE void dealRegister(const User& user);

    Q_INVOKABLE void dealSearch(const QString &searchContent);

    Q_INVOKABLE void dealAddFriend(const QString &account);

    Q_INVOKABLE void dealSendMsg(const Message& message);

    Q_INVOKABLE void dealDownload(const QString &path);

    void responseHandle();

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

private:
    QByteArray rawData(const QString &path);

    QString saveData(const QByteArray &bytes);

    static void appendUser(QQmlListProperty<User> *, User *);
    static int userCount(QQmlListProperty<User> *);
    static User *searchUser(QQmlListProperty<User> *, int);
    static void clearUsers(QQmlListProperty<User> *);

    void upload(const QString &path);

    void download(const Message &msg);

    QTcpSocket *socket;
    QString m_token;
    QList<User *> m_searchContent;
    Message *m_msg;
    User m_user;
    QString m_downloadPath;
    QList<QString> typeMap;
};

#endif // CLIENT_H
