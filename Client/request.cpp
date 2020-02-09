#include "request.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QVariantMap>
#include <QJsonParseError>
#include <QtDebug>

Request::Request(const QByteArray& bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if(doc.isEmpty()){
        qDebug() << "二进制数据未成功解析为jsonDoc" ;
        m_isNull = true;
    }
    else{
        m_json = doc.object();
        m_isNull = false;
    }
}

User Request::authur() const
{
    User user;
    if(m_json.contains("authur"))
        user = m_json["authur"].toObject();
    return user;
}

void Request::setAuthur(const User &authur)
{
    m_json["authur"] = authur.toJsonObject();
    m_isNull = false;
}

Message Request::msgContent() const
{
    Message msg;
    if(m_json.contains("msgContent"))
        msg = m_json["msgContent"].toObject();
    return msg;
}

void Request::setMsgContent(const Message &content)
{
    m_json["msgContent"] = content.toJsonObject();
    m_isNull = false;
}



QByteArray Request::toByteArray() const
{
    QJsonDocument doc;
    doc.setObject(m_json);
    return doc.toJson(QJsonDocument::Compact);
}

