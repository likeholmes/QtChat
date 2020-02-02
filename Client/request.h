#ifndef REQUEST_H
#define REQUEST_H

#include "message.h"
#include "user.h"

class Request: public QObject
{
    Q_OBJECT
public:
    enum Action{
      Login, Register, Search, Add, Send, Receive, Download
    };

    Request(QObject * parent = nullptr);

    Request(const QByteArray& bytes);

    QString token() const;
    void setToken(const QString &token);

    Action action() const;
    void setAction(Action action);

    User authur() const;
    void setAuthur(const User &authur);

    Message msgContent() const;
    void setMsgContent(const Message &content);

    qint64 downloadIndex() const;
    void setDownloadIndex(int index);

    QString searchContent() const;
    void setSearchContent(const QString& searchContent);

    QString addContent() const;
    void setAddContent(const QString & addContent);

    QJsonObject toJsonObject() const;

    QByteArray toByteArray() const;

private:
    QJsonObject m_json;
};

#endif // REQUEST_H
