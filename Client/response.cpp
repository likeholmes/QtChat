#include "response.h"

#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>

Response::Response(const QByteArray &bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if(doc.isEmpty()){
        qDebug()<<"json未解析成功";
        m_isNull = true;
    }else{
        m_json = doc.object();
        m_isNull = false;
    }

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
    m_isNull = false;
}

//结束搜索时应清空这部分内存
QList<User *> Response::searchResult() const
{
    QJsonArray jsons;
    if(m_json.contains("searchContent"))
        jsons = m_json["searchContent"].toArray();
    //qDebug()<<"jsonarray"<<jsons[0].toObject()["name"];
    QList<User *> list;
    for(QJsonValue value: jsons){
        User *user = new User(value.toObject());
        list.append(user);
    }
    return list;
}

//只获取6个
void Response::setSearchContent(const QList<User*> &list)
{
    QJsonArray jsons;
    for(int i = 0; i < (6 < list.size() ? 6 : list.size()); ++i){
        jsons << list.at(i)->toJsonObject();
    }
    m_json["searchContent"] = jsons;
    m_isNull = false;
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
    m_isNull = false;
}

QByteArray Response::toByteArray() const
{
    QJsonDocument doc;
    doc.setObject(m_json);
    return doc.toJson(QJsonDocument::Compact);
}

QList<Message> Response::msgContents() const
{
    QJsonArray jsons;
    if(m_json.contains("msgContents"))
        jsons = m_json["msgContents"].toArray();
    //qDebug()<<"jsonarray"<<jsons[0].toObject()["name"];
    QList<Message> list;
    for(QJsonValue value: jsons){
        Message msg(value.toObject());
        list.append(msg);
    }
    return list;
}

void Response::setMsgContents(const QList<Message> &list)
{
    QJsonArray jsons;
    for(int i = 0; i < list.size(); ++i){
        jsons << list.at(i).toJsonObject();
    }
    m_json["msgContents"] = jsons;
    m_isNull = false;
}

QList<User> Response::acceptContent() const
{
    QJsonArray jsons;
    if(m_json.contains("acceptContent"))
        jsons = m_json["acceptContent"].toArray();
    //qDebug()<<"jsonarray"<<jsons[0].toObject()["name"];
    QList<User> list;
    for(QJsonValue value: jsons){
        User user(value.toObject());
        list.append(user);
    }
    return list;
}

void Response::setAcceptContent(const QList<User> &list)
{
    QJsonArray jsons;
    for(int i = 0; i < list.size(); ++i){
        jsons << list.at(i).toJsonObject();
    }
    m_json["acceptContent"] = jsons;
    m_isNull = false;
}
