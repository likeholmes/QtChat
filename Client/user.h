#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)

    Q_PROPERTY(QString account READ account)

    Q_PROPERTY(QString avatarPath READ avatarPath WRITE setAvatarPath)

    Q_PROPERTY(QString describe READ describe WRITE setDescribe)

    Q_PROPERTY(QString password READ password WRITE setPassword)
public:
    explicit User(QObject *parent = nullptr);

    User(const QString &account, const QString &password, const QString &name,
         const QString &avatar, const QString &describe);

    User(const QJsonObject &json);

    User(const QByteArray &bytes);

    User(const User &user);

    User& operator= (const User& user);

    QString name() const;
    void setName(const QString &name);

    QString describe() const;
    void setDescribe(const QString &describe);

    QString password() const;
    void setPassword(const QString &password);

    QString avatarPath() const;
    void setAvatarPath(const QString &avatarPath);

    QString account() const;
    void setAccount(const QString &account);

    QByteArray avatarData(const QString &data);
    QByteArray avatarData() const;
    void setAvatarData(const QByteArray &avatarData);

    QByteArray toByteArray() const;

    QJsonObject toJsonObject() const;


signals:

};

#endif // USER_H
