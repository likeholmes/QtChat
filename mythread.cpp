#include "mythread.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QPixmap>
#include <QDateTime>
#include <QDir>
#include <QRandomGenerator>
#include <QSqlQuery>

MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    srand(QDateTime::currentSecsSinceEpoch()); //会不会溢出。。。
}

MyThread::~MyThread()
{

}

// 【注意：处理套接字的任何时候，套接字都有可能从客户端关闭】
void MyThread::run()
{
    //qint64 maxlen = 131072;
    for (auto socket: m_sockets){
        auto t_socket = socket;
        m_sockets.removeOne(socket);
        if(t_socket->bytesAvailable() > 0) //若套接字内有可读取的数据
        {
            //从套接字中取出数据进行处理
            QByteArray bytes;
            QDataStream in(t_socket);
            in.startTransaction();
            //一次读取完整数据
            in >> bytes;
            if(!in.commitTransaction())
            {
                //不处理
                return;
            }
            handleRequest(bytes, t_socket);
            m_sockets.push_back(t_socket); //处理完请求后将套接字加入队尾
            return;
        }
    }
}
// 【注意：处理套接字的任何时候，套接字都有可能从客户端关闭】

int MyThread::getSocketCount()
{
    return m_sockets.size();
}

void MyThread::addSocket(qintptr socketDescriptor)
{
    MySocket *socket = new MySocket();
    if(!socket->setSocketDescriptor(socketDescriptor)){
        //抛出错误
        emit error(socket->error());
        return;
    }
    m_sockets.push_front(socket);
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(removeSocket(socket)));
}

// 【线程什么时候会使用这个槽】
void MyThread::removeSocket(MySocket *socket)
{
    //是否要判断错误,以及删除顺序是否有误？
    delete socket;
    m_sockets.removeOne(socket);

}
// 【线程什么时候会使用这个槽】

void MyThread::handleRequest(QByteArray &data, MySocket *socket)
{
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();
    if(json.contains("action") && json["action"].isString())
    {
        QString action = json["action"].toString();
        if(action == "register"){ //注册
            dealRegister(json, socket);
        }else if(action == "login"){ //登录
            dealRegister(json, socket);
        }else if(action == "lookup"){ //查找

        }else if(action == "sendMsg"){
            //dealSend(json, socket);
        }
    }
}

// 【处理用户注册】
void MyThread::dealRegister(QJsonObject &json, MySocket *socket)
{
    QSqlQuery query;
    QString name = json["name"].toString();
    QString rawPassword = json["password"].toString();
    QString password = encrypt(rawPassword);
    QString account = QDateTime::currentSecsSinceEpoch() + tr("") + rand()%100; //随机生成账号
    QString sql = QString("INSERT INTO users (account, password, name) VALUES ('%1', '%2', '%3')")
               .arg(account).arg(password).arg(name);
    QString describe;
    QByteArray avatarData;
    QString avatarLocalPath;
    //个性签名
    if(json.contains("describe") && json["describe"].isString()){
        describe = json["describe"].toString();
        sql = QString("INSERT INTO users (account, password, name, describe) VALUES ('%1', '%2', '%3', '%4')")
                .arg(account).arg(password).arg(name).arg(describe);
    }
    //头像数据，需保存到头像表中，保存到文件中
    if(json.contains("avatarData")){
        avatarData = json["avatarData"].toVariant().toByteArray();
        QPixmap avatar;
        avatar.loadFromData(avatarData); //文件后缀如何规定，是否用自动检测文件头的方式
        QString format = json["format"].toString();
        QString fileName = tr("../Server/resource/Avator") +   //相对路径
                QDateTime::currentMSecsSinceEpoch() + tr(".") + format;
        avatar.save(QDir(fileName).absolutePath());
        avatarLocalPath = json["avatarLocalPath"].toString();  //绝对路径
        //存入头像表中，获取所在行的索引
        query.exec(QString("INSERT INTO avatars (spath, lpath) VALUES ('%1', '%2')") //[1]
                   .arg(fileName).arg(avatarLocalPath));
        query.exec("SELECT * FROM avatars"); //[2]
        int avatarId = query.size(); //假设每条记录都加在最后一条
        if (describe.size() > 0){
            sql = QString("INSERT INTO users (account, password, name, describe, avatar) VALUES ('%1', '%2', '%3', '%4', %5)")
                    .arg(account).arg(password).arg(name).arg(describe).arg(avatarId);
        }else{
            sql = QString("INSERT INTO users (account, password, name, avatar) VALUES ('%1', '%2', '%3', %4)")
                    .arg(account).arg(password).arg(name).arg(avatarId);
        }
    }
    query.exec(sql);  //[3]
    generateResponse("register", "success");
}
// 【处理用户注册】
