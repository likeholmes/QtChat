#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QJsonObject>

class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(QObject *parent = nullptr);
    Message(const QByteArray &json);

    QString action() const;
    void setAction(const QString &action);

    QString type() const;
    void setType(const QString &type);

    QString authur() const;
    void setAuthur(const QString &authur);

    QByteArray content() const;
    void setContent(const QByteArray &content);

    QString recipiant() const;
    void setRecipient(const QString &recipient);

    QJsonObject message() const;

    virtual QByteArray toByteArray() const;

signals:

private:
    QJsonObject m_json;
};

#endif // MESSAGE_H
