#include "request.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>
#include <QJsonParseError>

Request::Request(const QByteArray& bytes)
{
    QJsonDocument doc;
    doc.fromBinaryData(bytes);
    m_json = doc.object();
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
}



QByteArray Request::toByteArray() const
{
    QJsonDocument doc;
    doc.setObject(m_json);
    return doc.toBinaryData();
}

