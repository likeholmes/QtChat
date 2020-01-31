#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client: public QObject
{
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);

    void dealError(QAbstractSocket::SocketError error);

    void dealLogin(const QString &account, const QString & password);

    void dealRegister(const QString &authur, const QString &password, const QString &describe,
                  const QString &avatar);

    void dealSearch(const QString &user);

    void dealAddFriend(const QString &account);

    void dealSendMsg(const QString &account, const QString &type, const QString &path);

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

private:
    QByteArray rawData(const QString &path);

    QString saveData(const QByteArray &bytes);

    QTcpSocket socket;
    QString m_token;
    QString m_account;
    QString m_avatar;
    QString m_name;
    QString m_describe;
};

#endif // CLIENT_H
