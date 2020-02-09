#include "user.h"

#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPixmap>

User::User(const QJsonObject &json){
    m_isgroup = 0;
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
    if(json.contains("isGroup"))
        m_isgroup = json["isGroup"].toInt();
}

User::User(const User &user, QObject *parent): QObject(parent){
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
    m_isgroup = user.isGroup();
    return *this;
}

void User::loadDataFromPath(){
    if(!m_avatarPath.isEmpty()){
        QFile file(m_avatarPath);
        file.open(QIODevice::ReadOnly);
        m_avatarData = file.readAll().toBase64();
        file.close();
    }
}

void User::saveAvatar(const QString& basePath)
{
    //获取avatarPath的文件名
    QString localPath = basePath + "avatar/" + getFileName();
    qDebug() << localPath;
    QByteArray bytes;
    if(!m_avatarData.isEmpty()){
        bytes = QByteArray::fromBase64(m_avatarData.toUtf8());
        QFile file(localPath);
        if(!file.exists()){
            file.open(QIODevice::WriteOnly);
            file.write(bytes);
            file.close();
        }
        m_avatarPath = localPath;
    }

}

QJsonObject User::toJsonObject() const
{
    QJsonObject json;
    QString name = m_name;
    if(!m_avatarData.isEmpty())
        json["avatarData"] = m_avatarData;
    if(!m_avatarPath.isEmpty())
        json["avatarPath"] = m_avatarPath;
    if(!m_name.isEmpty()){
        json["name"] = m_name;
    }
    if(!m_account.isEmpty()){
        json["account"] = m_account;
    }
    if(!m_password.isEmpty()){
        json["password"] = m_password;
    }
    if(!m_describe.isEmpty())
        json["describe"] = m_describe;
    if(m_isgroup > 0){
        json["isGroup"] = m_isgroup;
    }
    return json;
}

//可能其它平台的存储结构不一样
QString User::getFileName()
{
    return m_avatarPath.right(m_avatarPath.length() - m_avatarPath.lastIndexOf('/') - 1);
}
