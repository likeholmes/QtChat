#include "response.h"

#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Response::Response(const QByteArray &bytes)
{
    QJsonDocument doc;
    doc.fromBinaryData(bytes);
    m_json = doc.object();
}

User Response::authur() const
{
    User user;
    if(m_json.contains("authur"))
        user = m_json["authur"].toObject();
    return user;
}

void Response::setAuthur(const User &authur)
{
    m_json["authur"] = authur.toJsonObject();
}

QList<User *> Response::searchResult() const
{
    QJsonArray jsons;
    if(m_json.contains("searchResult"))
        jsons = m_json["searchResult"].toArray();
    QList<User *> list;
    for(QJsonValue value: jsons){
        User *user = new User(value.toObject());
        list.append(user);
    }
    return list;
}

void Response::setSearchContent(const QList<User *> &list)
{
    QJsonArray jsons;
    for(User *user: list){
        jsons.append(user->toJsonObject());
    }
    m_json["searchContent"] = jsons;
}

Message Response::msgContent() const
{
    Message msg;
    if(m_json.contains("msgContent"))
        msg = m_json["msgContent"].toObject();
    return msg;
}

void Response::setMsgContent(const Message &msg)
{
    m_json["msgContent"] = msg.toJsonObject();
}

QByteArray Response::toByteArray() const
{
    QJsonDocument doc;
    doc.setObject(m_json);
    return doc.toBinaryData();
}

