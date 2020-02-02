#include "response.h"

#include <QVariant>

QString Response::action() const
{
    if(m_json.contains("action"))
        return m_json["action"].toString();
    return NULL;
}

void Response::setAction(const QString &action)
{
    m_json["action"] = action;
}

QString Response::authur() const
{
    if(m_json.contains("authur"))
        return m_json["authur"].toString();
    return NULL;
}

void Response::setAuthur(const QString &authur)
{
    m_json["authur"] = authur;
}

QString Response::recipiant() const
{
    if(m_json.contains("recipient"))
        return m_json["recipient"].toString();
    return NULL;
}

void Response::setRecipient(const QString &recipient)
{
    m_json["recipient"] = recipient;
}

QString Response::type() const
{
    if(m_json.contains("type"))
        return m_json["type"].toString();
    return NULL;
}

void Response::setType(const QString &type)
{
   m_json["type"] = type;
}

QByteArray Response::content() const
{
    if(m_json.contains("content"))
    {
        return m_json["content"].toVariant().toByteArray();
    }
    return NULL;
}

void Response::setContent(const QByteArray &content)
{
    m_json["content"] = QVariant(content).toJsonValue();
}

