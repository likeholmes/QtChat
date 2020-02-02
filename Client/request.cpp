#include "request.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>

Request::Request(const QByteArray& bytes)
{
    QJsonDocument doc;
    doc.fromBinaryData(bytes);
    m_json = doc.object();
}

QString Request::token() const
{
    if(m_json.contains("token"))
        return m_json["token"].toString();
    return NULL;
}

void Request::setToken(const QString &token)
{
   m_json["token"] = token;
}

QString Request::format() const
{
    if(m_json.contains("format"))
        return m_json["format"].toString();
    return NULL;
}

void Request::setFormat(const QString &format)
{
    m_json["format"] = format;
}


QString Request::password() const
{
    if(m_json.contains("password"))
        return m_json["password"].toString();
    return NULL;
}

void Request::setPassword(const QString &password)
{
    m_json["password"] = password;
}

QString Request::action() const
{
    //return m_body["action"].toString();
    if(m_json.contains("action"))
        return m_json["action"].toString();
    return NULL;
}

void Request::setAction(const QString &action)
{
    m_json["action"] = action;
}

QString Request::authur() const
{
    if(m_json.contains("authur"))
        return m_json["authur"].toString();
    return NULL;
}

void Request::setAuthur(const QString &authur)
{
    m_json["authur"] = authur;
}

QString Request::recipiant() const
{
    if(m_json.contains("recipient"))
        return m_json["recipient"].toString();
    return NULL;
}

void Request::setRecipient(const QString &recipient)
{
    m_json["recipient"] = recipient;
}

QString Request::type() const
{
    if(m_json.contains("type"))
        return m_json["type"].toString();
    return NULL;
}

void Request::setType(const QString &type)
{
   m_json["type"] = type;
}

QByteArray Request::content() const
{
    if(m_json.contains("content"))
    {
        return m_json["content"].toVariant().toByteArray();
    }
    return NULL;
}

void Request::setContent(const QByteArray &content)
{
    m_json["content"] = QVariant(content).toJsonValue();
}

QByteArray Request::toByteArray() const
{
    QJsonDocument doc;
    doc.setObject(m_json);

    return doc.toBinaryData();
}

