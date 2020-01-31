#include "request.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>

Request::Request(QObject *parent):
    Message(parent)
{

}

Request::Request(const QByteArray& bytes):
    Message(bytes)
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

QByteArray Request::toByteArray() const
{
    QJsonObject json = message();

    for (QString key : m_json.keys()) {
        json[key] = m_json[key];
    }

    QJsonDocument doc;
    doc.setObject(json);

    return doc.toBinaryData();
}

