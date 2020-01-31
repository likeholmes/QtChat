#ifndef RESPONSE_H
#define RESPONSE_H

#include "message.h"

#include <QObject>

class Response: public Message
{
    Q_OBJECT
public:

    enum State {
        SUCCESS, FAILURE
    };

    Response(QObject *parent);

    Response(const QByteArray &bytes);

    QString response() const;
    void setResponse(const QString &response);

    QString name() const;
    void setName(const QString &name);

    QString describe() const;
    void setDescribe(const QString &describe);

    QString token() const;
    void setToken(const QString &token);

private:
    QJsonObject m_json;

};

#endif // RESPONSE_H
