#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)

    Q_PROPERTY(QString account READ account WRITE setAccount)

    Q_PROPERTY(QString avatarPath READ avatarPath WRITE setAvatarPath)

    Q_PROPERTY(QString describe READ describe WRITE setDescribe)

    Q_PROPERTY(QString password READ password WRITE setPassword)

    Q_PROPERTY(int isgroup READ isGroup WRITE setIsGroup)
public:
    explicit User(QObject *parent = nullptr):QObject(parent){m_isgroup = 0;}

    User(const QJsonObject &json);

    User(const User &user, QObject *parent = nullptr);

    User& operator= (const User& user);

    QString name() const{
        return m_name;
    }
    void setName(const QString &name){
        m_name = name;
    }

    QString describe() const{
        return m_describe;
    }
    void setDescribe(const QString &describe){
        m_describe = describe;
    }

    QString password() const{
        return m_password;
    }
    void setPassword(const QString &password){
        m_password = password;
    }

    QString avatarPath() const{
        return m_avatarPath;
    }
    void setAvatarPath(const QString &avatarPath){
        m_avatarPath = QString(avatarPath).replace('\\', '/').remove("file:///");
    }

    QString account() const{
        return m_account;
    }
    void setAccount(const QString &account){
        m_account = account;
    }

    void loadDataFromPath();

    void saveAvatar(const QString& basePath);

    QString avatarData() const{
        return m_avatarData;
    }

    QJsonObject toJsonObject() const;

    int isGroup() const{
        return m_isgroup;
    }

    void setIsGroup(int isgroup){
        m_isgroup = isgroup;
    }

signals:

private:
    QString getFileName();

    QString m_account;
    QString m_name;
    QString m_describe;
    QString m_password;
    QString m_avatarPath;
    QString m_avatarData;
    int m_isgroup;

};

#endif // USER_H
