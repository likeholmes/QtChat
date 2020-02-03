#include "user.h"

#include <QJsonObject>
#include <QFile>
#include <QDir>

User::User(const QJsonObject &json){
    if(json.contains("account"))
        m_account = json["account"].toString();
    if(json.contains("password"))
        m_password = json["password"].toString();
    if(json.contains("name"))
        m_name = json["name"].toString();
    if(json.contains("avatarPath"))
        m_avatarPath = json["avatarPath"].toString();
    if(json.contains("avatarData"))
        m_avatarData = json["avatarData"].toString();
    if(json.contains("describe"))
        m_describe = json["describe"].toString();
}

User::User(const User &user){
    *this = user;
}

User& User::operator=(const User &user)
{
    m_name = user.name();
    m_account = user.account();
    m_password = user.password();
    m_describe = user.describe();
    m_avatarData = user.avatarData();
    m_avatarPath = user.avatarPath();
    return *this;
}

void User::loadDataFromPath(){
    if(!m_avatarPath.isNull()){
        QFile file(m_avatarPath);
        file.open(QIODevice::ReadOnly);
        m_avatarData = file.readAll();
        file.close();
    }
}

void User::saveAvatar()
{
    //获取avatarPath的文件名
    QString localPath = QDir("avatar/" + getFileName()).absolutePath();
    QByteArray bytes;
    if(!m_avatarData.isNull()){
        bytes = m_avatarData.toLocal8Bit();
        QFile file(localPath);
        if(!file.exists()){
            file.open(QIODevice::WriteOnly);
            file.write(bytes);
            file.close();
        }
    }
    setAvatarPath(localPath);
}

QJsonObject User::toJsonObject() const
{
    QJsonObject json;
    if(!m_avatarData.isNull())
        json["avatarData"] = m_avatarData;
    if(!m_avatarPath.isNull())
        json["avatarPath"] = m_avatarPath;
    if(!m_name.isNull())
        json["name"] = m_name;
    if(!m_account.isNull())
        json["account"] = m_account;
    if(!m_password.isNull())
        json["password"] = m_password;
    if(!m_describe.isNull())
        json["describe"] = m_describe;
    return json;
}
