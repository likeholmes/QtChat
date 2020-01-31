#include "message.h"

#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>

Message::Message(QObject *parent) : QObject(parent)
{

}

Message::Message(const QByteArray& bytes){
    QJsonDocument doc;
    doc.fromBinaryData(bytes);
    m_json = doc.object();
}

QString Message::action() const
{
    //return m_body["action"].toString();
    if(m_json.contains("action"))
        return m_json["action"].toString();
    return NULL;
}

void Message::setAction(const QString &action)
{
    m_json["action"] = action;
}

QString Message::authur() const
{
    if(m_json.contains("authur"))
        return m_json["authur"].toString();
    return NULL;
}

void Message::setAuthur(const QString &authur)
{
    m_json["authur"] = authur;
}

QString Message::recipiant() const
{
    if(m_json.contains("recipient"))
        return m_json["recipient"].toString();
    return NULL;
}

void Message::setRecipient(const QString &recipient)
{
    m_json["recipient"] = recipient;
}

QString Message::type() const
{
    if(m_json.contains("type"))
        return m_json["type"].toString();
    return NULL;
}

void Message::setType(const QString &type)
{
   m_json["type"] = type;
}

QByteArray Message::content() const
{
    if(m_json.contains("content"))
    {
        return m_json["content"].toVariant().toByteArray();
    }
    return NULL;
}

void Message::setContent(const QByteArray &content)
{
    m_json["content"] = QVariant(content).toJsonValue();
}

QJsonObject Message::message() const
{
    return m_json;
}
