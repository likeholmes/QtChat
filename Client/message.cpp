#include "message.h"

#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

Message::Message(QObject *parent) : QObject(parent)
{
    m_filesize = 0;
    m_fileIndex = 0;
}

Message::Message(const QJsonObject &json){
    m_filesize = 0;
    m_fileIndex = 0;
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
    if(json.contains("timeStamp"))
        m_timeStamp = json["timeStamp"].toString();

}

Message::Message(const Message &msg, QObject *parent): QObject(parent){
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
    m_timeStamp = msg.timeStamp();
    return *this;
}

//风险：万一message对象之前使用了，但是里面的数据没有变，就会搞错
void Message::dealFile(){
    int chunk = 655360;
    if(!filePath().isEmpty()){
        QFile file(filePath());
        if(file.exists()){
            QString aname = getFileName();
            qDebug() << "fileName:" <<aname;
            if(aname.contains("chat") && aname.contains("___")){
                //从服务器端下载下来存在本地的文件
                aname.remove(0, 4);
                QStringList list = aname.split("___");
                setFileIndex(list[0].toInt());
                setFileName(list[1]);
            }else{
                setFileName(aname);
            }
            setFileSize(file.size());
            file.open(QIODevice::ReadOnly);
            if(fileSize() < chunk){
                setTextMsg(file.read(chunk).toBase64());
            }
            file.close();
        }else{
            //文件不存在或路径不正确
        }
    }
}

void Message::saveSmallFile(const QString& basePath, Place place){
    qDebug() << "saveSmallFile---";
    QStringList ss;
    ss.push_back("none");
    ss.push_back("text");
    ss.push_back("picture");
    ss.push_back("file");

    if(type() != Text){   
        QString localPath = place == Server ?
                    basePath + ss[type()] + "/" + fileName() :
                    basePath + ss[type()] + "/chat" + QString().setNum(fileIndex()) + "___" + fileName();
        qDebug() << "localPath:"<<localPath;
        if(fileSize() < 655360) {
            QFile file(localPath);
            if(!file.exists()){
                qDebug() << "文件不存在";
                QByteArray bytes = QByteArray::fromBase64(m_textMsg.toUtf8());
                qDebug() << "已写入bytes";
                file.open(QIODevice::WriteOnly);
                file.write(bytes);
                file.close();
                qDebug() << "关闭文件";
           }
        }
        if(place == Client)
            setTextMsg(localPath);
        setFilePath(localPath);
    }
    qDebug() << "saveSmallFile+++";
}

QJsonObject Message::toJsonObject() const
{
    QJsonObject json;
    json["type"] = m_type;
    if(!m_textMsg.isEmpty())
        json["textMsg"] = m_textMsg;
    if(!m_fileName.isEmpty())
        json["fileName"] = m_fileName;
    if(!m_authur.isEmpty())
        json["authur"] = m_authur;
    if(!m_recipient.isEmpty())
        json["recipient"] = m_recipient;
    if(!m_filePath.isEmpty())
        json["filePath"] = m_filePath;
    if(!m_timeStamp.isEmpty())
        json["timeStamp"] = m_timeStamp;
    if(m_filesize > 0)
        json["fileSize"] = m_filesize;
    if(m_fileIndex > 0)
        json["fileIndex"] = m_fileIndex;
    return json;
}

void Message::setTimeStamp()
{
    m_timeStamp = QDateTime::currentDateTime().toString(Qt::ISODate);
}

QString Message::getFileName()
{
    return filePath().right(filePath().length() - filePath().lastIndexOf('/') - 1);
}





