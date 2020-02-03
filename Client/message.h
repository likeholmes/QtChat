#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QJsonObject>

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type WRITE setType)

    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)

    Q_PROPERTY(QString authur READ authur WRITE setAuthur)

    Q_PROPERTY(QString recipient READ recipient WRITE setRecipient)

    Q_PROPERTY(QString fileIndex READ fileIndex WRITE setFileIndex)

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)

    Q_PROPERTY(QString fileSize READ fileSze WRITE setFileSize)

    Q_PROPERTY(QString textMsg READ textMsg WRITE setTextMsg)

public:

    explicit Message(QObject *parent = nullptr);

    Message(const QJsonObject &json);

    Message(const QString &msg){
        m_textMsg = msg;
        m_type = Text;
    }

    Message(const Message &msg);

    Message& operator= (const Message &msg);

    enum Type{
        None, Text, Picture, File
    };

    Q_ENUM(Type)

    Type type() const{
        return m_type;
    }

    void setType(Type type){
        m_type = type;
    }

    QString textMsg() const{
        return m_textMsg;
    }

    qint64 fileSize() const{
        return m_filesize;
    }
    void setFileSize(qint64 size){
        m_filesize = size;
    }

    QString fileName() const{
        return m_fileName;
    }
    void setFileName(const QString& fileName){
        m_fileName = fileName;
    }

    QString filePath() const{
        return m_filePath;
    }

    void setFilePath(const QString& filePath){
        m_filePath = filePath;
    }

    int fileIndex() const{
        return m_fileIndex;
    }

    void setFileIndex(int fileIndex){
        m_fileIndex = fileIndex;
    }

    QString authur() const{
        return m_authur;
    }
    void setAuthur(const QString & authur){
        m_authur = authur;
    }

    QString recipient() const{
        return m_recipient;
    }
    void setRecipient(const QString &recipient){
        m_recipient = recipient;
    }

    QString timeStamp() const{
        return m_timeStamp;
    }

    void setTimeStamp(const QString &timeStamp){
        m_timeStamp = timeStamp;
    }

    void setTimeStamp();

    void dealFile();

    QJsonObject toJsonObject() const;

signals:

private:
    QString m_timeStamp;
    QString m_textMsg;
    Type m_type;
    qint64 m_filesize;
    QString m_fileName;
    QString m_filePath;
    int m_fileIndex;
    QString m_authur;
    QString m_recipient;
};

#endif // MESSAGE_H
