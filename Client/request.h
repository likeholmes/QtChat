#ifndef REQUEST_H
#define REQUEST_H

#include "message.h"
#include "user.h"

class Request: public QObject
{
    Q_OBJECT
public:
    enum Action{
      None, Login, Register, Search, Add, Send, Receive, Download
    };

    Request(QObject * parent = nullptr):QObject(parent){}

    Request(const QByteArray& bytes);

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

    int downloadIndex() const{
        if(m_json.contains("downloadIndex"))
            return m_json["downloadIndex"].toInt();
        return 0;
    }
    void setDownloadIndex(int index){
        m_json["downloadIndex"] = index;
    }

    QString searchContent() const{
        if(m_json.contains("searchContent"))
            return m_json["searchContent"].toString();
        return nullptr;
    }
    void setSearchContent(const QString& searchContent){
        m_json["searchContent"] = searchContent;
    }

    QString addContent() const{
        if(m_json.contains("addContent"))
            return m_json["addContent"].toString();
        return nullptr;
    }
    void setAddContent(const QString & addContent){
        m_json["addContent"] = addContent;
    }

    QJsonObject toJsonObject() const{
        return m_json;
    }

    User authur() const;
    void setAuthur(const User &authur);

    Message msgContent() const;
    void setMsgContent(const Message &content);

    QByteArray toByteArray() const;

private:
    QJsonObject m_json;
};

#endif // REQUEST_H
