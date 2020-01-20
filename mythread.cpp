#include "mythread.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
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
            dealLogin(json, socket);
        }else if(action == "lookup"){ //查找
            dealSearch(json, socket);
        }else if(action == "sendMsg"){
            dealSend(json, socket); //发送消息
        }else if(action == "addFriend"){
            dealAddFriend(json, socket);  //添加好友
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
        //avatarData = json["avatarData"].toVariant().toByteArray();
        QPixmap avatar = json["avatarData"].toVariant().value<QPixmap>();
        //avatar.loadFromData(avatarData); //文件后缀如何规定，是否用自动检测文件头的方式
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
    generateResponse("register", "success"); //处理响应信息，可以作为返回值
}
// 【处理用户注册】

void MyThread::dealLogin(QJsonObject &json, MySocket *socket)
{
    QString account = json["account"].toString();
    QString rawPassword = json["password"].toString();
    QString password = encrypt(rawPassword); //经过某种加密后
    QSqlQuery query;
    QString response = "fail";
    QString authur = "";
    QString token = "";
    query.exec(QString("SELECT * FROM users WHERE account = '%1' and password = '%2'").
               arg(account).arg(password));
    if(query.next()){  //找到用户之后；颁发某种通行证;
        //取出id,作为之后的索引返回给客户端，然后客户端再每次请求的时候发送该字段，安全暂不考虑
        int id = query.value("id").toInt();
        authur = query.value("name").toString();
        token = generateToken(id);
        response = "success";
    }
    generateResponse("login", response, token, authur); //type字段在不是传输消息时，可以存放一些简短的响应消息
}

void MyThread::dealSearch(QJsonObject &json, MySocket *socket)
{
    int id = decodeToken(json["token"].toString());
    QString account;
    QString name;
    QSqlQuery query;
    QJsonArray users;
    if (json.contains("account") && json["account"].isString()){
        account = json["account"].toString();
        query.exec(QString("SELECT * FROM users WHERE account = '%1'").arg(account));
        //QJsonObject obj = query.next()
        if(query.next()){
            QJsonObject obj;
            int idx = query.value("id").toInt();
            obj["name"] = query.value("name").toString();
            obj["account"] = query.value("account").toString();
            obj["isgroup"] = query.value("isgroup").toInt();
            obj["describe"] = query.value("describe").toString();
            int avatarId = query.value("avatar").toInt();
            //获取头像信息
            QSqlQuery nquery;
            nquery.exec(QString("SELECT * FROM avatars WHERE id = %1").arg(avatarId)); //在一个查询内部再一个查询不知道可不可以。。。
            if(nquery.next()){
                QString path = nquery.value("spath").toString();
                QPixmap avatar(QDir(path).absolutePath());
                obj["avatarData"] = QVariant(avatar).toJsonValue(); //这中间是否经过几重转换...
                obj["format"] = path.split(".").last();
            }

            nquery.exec(QString("SELECT * FROM contacts WHERE user1 = %1 and user2 = %2")
                        .arg(id).arg(idx));
            if(nquery.next()){
                obj["iscontact"] = 1;
            }
            users.push_back(obj);
        }
    }else if (json.contains("name") && json["name"].isString()) {
        name = json["name"].toString();
        query.exec(QString("SELECT * FROM users WHERE name LIKE '%1'").arg(tr("%") + name + "%"));
        while (query.next()) {
            QJsonObject obj;
            int idx = query.value("id").toInt();
            obj["name"] = query.value("name").toString();
            obj["account"] = query.value("account").toString();
            obj["isgroup"] = query.value("isgroup").toInt();
            obj["describe"] = query.value("describe").toString();
            int avatarId = query.value("avatar").toInt();
            //获取头像信息
            QSqlQuery nquery;
            nquery.exec(QString("SELECT * FROM avatars WHERE id = %1").arg(avatarId)); //在一个查询内部再一个查询不知道可不可以。。。
            if(nquery.next()){
                QString path = nquery.value("spath").toString();
                QPixmap avatar(QDir(path).absolutePath());
                obj["avatarData"] = QVariant(avatar).toJsonValue(); //这中间是否经过几重转换...
                obj["format"] = path.split(".").last();
            }
            //这一步可检查该用户是否为群组
            nquery.exec(QString("SELECT * FROM contacts WHERE user1 = %1 and user2 = %2")
                        .arg(id).arg(idx));
            if(nquery.next()){
                obj["iscontact"] = 1;
            }
            users.push_back(obj);
        }
    }
    generateResponse("search", "success", "", "", "", "", QVariant(users)); //QVariant多层嵌套可还行
}
