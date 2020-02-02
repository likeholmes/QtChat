#ifndef RESPONSE_H
#define RESPONSE_H

#include "message.h"
#include "user.h"

class Response: public QObject
{
    Q_OBJECT
public:

    enum State {
        FAILURE, SUCCESS
    };

    enum Action{
      Login, Register, Search, Add, Send, Receive, Download
    };

    Response(QObject *parent);

    Response(const QByteArray &bytes);

    State response() const;
    void setResponse(State response);

    QString token() const;
    void setToken(const QString &token);

    Action action() const;
    void setAction(Action action);

    User authur() const;
    void setAuthur(const User &authur);

    QByteArray content() const;
    void setContent(const QByteArray &content);

    QList<User*> searchResult() const;

    Message msgContent() const;

    QJsonObject toJsonObject() const;

private:
    QJsonObject m_json;

};

#endif // RESPONSE_H
