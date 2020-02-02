#include "message.h"

#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>

Message::Message(QObject *parent) : QObject(parent)
{

}

Message::Message(const QJsonObject &json){
    if(json.contains("textMsg"))
        m_textMsg = json["textMsg"].toString();
    if(json.contains("type"))
        m_type = Type(json["type"].toInt());
    if(json.contains("fileSize"))
        m_filesize = json["fileSize"].toInt();
    if(json.contains("fileName"))
        m_fileName = json["fileName"].toString();
    if(json.contains("filePath"))
        m_filePath = json["filePath"].toString();
    if(json.contains("fileIndex"))
        m_fileIndex = json["fileIndex"].toInt();
    if(json.contains("authur"))
        m_authur = json["authur"].toString();
    if(json.contains("recipient"))
        m_recipient = json["recipient"].toString();
}

Message::Message(const Message &msg){
    *this = msg;
}

Message& Message::operator= (const Message &msg){
    m_type = msg.type();
    m_fileName = msg.fileName();
    m_filePath = msg.filePath();
    m_filesize = msg.fileSize();
    m_fileIndex = msg.fileIndex();
    m_textMsg = msg.textMsg();
    m_authur = msg.authur();
    m_recipient = msg.recipient();
    return *this;
}

//风险：万一message对象之前使用了，但是里面的数据没有变，就会搞错
void Message::dealFile(){
    int chunk = 65536;
    if(m_filePath.length() > 0){
        QFile file(m_filePath);
        m_fileName = file.fileName();
        m_filesize = file.size();
        file.open(QIODevice::ReadOnly);
        if(m_filesize < chunk){
            m_textMsg = file.read(chunk);
        }
        file.close();
    }
}

QJsonObject Message::toJsonObject() const
{
    QJsonObject json;
    json["type"] = m_type;
    if(m_textMsg.length() > 0)
        json["textMsg"] = m_textMsg;
    if(m_fileName.length() > 0)
        json["fileName"] = m_fileName;
    if(m_authur.length() > 0)
        json["authur"] = m_authur;
    if(m_recipient.length() > 0)
        json["recipient"] = m_recipient;
    if(m_filePath.length() > 0)
        json["filePath"] = m_filePath;
    if(m_filesize > 0)
        json["fileSize"] = m_filesize;
    if(m_fileIndex > 0)
        json["fileIndex"] = m_fileIndex;
    return json;
}






