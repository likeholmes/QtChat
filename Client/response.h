#ifndef RESPONSE_H
#define RESPONSE_H

#include "message.h"
#include "user.h"

class Response: public QObject
{
    Q_OBJECT
public:

    enum State {
        NOSTATE, FAILURE, SUCCESS
    };

    enum Action{
      None, Login, Register, Search, Add, Send, Receive, Download
    };

    Response(QObject *parent = nullptr):QObject(parent){}

    Response(const QByteArray &bytes);

    State response() const{
        if(m_json.contains("response"))
            return State(m_json["response"].toInt());
        return NOSTATE;
    }
    void setResponse(State response){
        m_json["response"] = response;
    }

    QString token() const{
        if(m_json.contains("token"))
            return m_json["token"].toString();
        return nullptr;
    }
    void setToken(const QString &token){
        m_json["token"] = token;
    }

    Action action() const{
        if(m_json.contains("action"))
            return Action(m_json["action"].toInt());
        return None;
    }
    void setAction(Action action){
        m_json["action"] = action;
    }

    QJsonObject toJsonObject() const{
        return m_json;
    }

    User authur() const;
    void setAuthur(const User &authur);

    QList<User*> searchResult() const;
    void setSearchContent(const QList<User*> &list);

    Message msgContent() const;
    void setMsgContent(const Message &msg);

    QByteArray toByteArray() const;

private:
    QJsonObject m_json;

};

#endif // RESPONSE_H
