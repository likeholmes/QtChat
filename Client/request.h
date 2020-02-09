#ifndef REQUEST_H
#define REQUEST_H

#include "message.h"
#include "user.h"

class Request: public QObject
{
    Q_OBJECT
public:
    enum Action{
      None, Login, Register, Search, Add, Send, Receive, Download, Accept
    };

    Request(QObject * parent = nullptr):QObject(parent){m_isNull = true;}

    Request(const QByteArray& bytes);

    Request(const Request& res, QObject * parent = nullptr):QObject(parent){
        m_json = res.toJsonObject();
        m_isNull = res.isNull();
    }

    Request& operator=(const Request& res){
        m_json = res.toJsonObject();
        m_isNull = res.isNull();
        return *this;
    }

    QString token() const{
        if(m_json.contains("token"))
            return m_json["token"].toString();
        return nullptr;
    }
    void setToken(const QString &token){
        m_json["token"] = token;
        m_isNull = false;
    }

    Action action() const{
        if(m_json.contains("action"))
            return Action(m_json["action"].toInt());
        return None;
    }
    void setAction(Action action){
        m_json["action"] = action;
        m_isNull = false;
    }

    int downloadIndex() const{
        if(m_json.contains("downloadIndex"))
            return m_json["downloadIndex"].toInt();
        return 0;
    }
    void setDownloadIndex(int index){
        m_json["downloadIndex"] = index;
        m_isNull = false;
    }

    QString searchContent() const{
        if(m_json.contains("searchContent"))
            return m_json["searchContent"].toString();
        return nullptr;
    }
    void setSearchContent(const QString& searchContent){
        m_json["searchContent"] = searchContent;
        m_isNull = false;
    }

    QString addContent() const{
        if(m_json.contains("addContent"))
            return m_json["addContent"].toString();
        return nullptr;
    }
    void setAddContent(const QString & addContent){
        m_json["addContent"] = addContent;
        m_isNull = false;
    }

    QJsonObject toJsonObject() const{
        return m_json;
    }

    User authur() const;
    void setAuthur(const User &authur);

    Message msgContent() const;
    void setMsgContent(const Message &content);

    QByteArray toByteArray() const;

    bool isNull() const{
        return m_isNull;
    }


private:
    QJsonObject m_json;
    bool m_isNull;
};

#endif // REQUEST_H
