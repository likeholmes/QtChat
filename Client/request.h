#ifndef REQUEST_H
#define REQUEST_H

#include "message.h"

class Request: public Message
{
    Q_OBJECT
public:
    Request(QObject * parent = nullptr);

    Request(const QByteArray& bytes);

    QString token() const;
    void setToken(const QString &token);

    QString format() const;
    void setFormat(const QString &format);

    QString password() const;
    void setPassword(const QString &password);

    QString name() const;
    void setName(const QString &name);

    QString describe() const;
    void setDescribe(const QString &describe);

    QByteArray toByteArray() const override;

private:
    QJsonObject m_json;
};

#endif // REQUEST_H
