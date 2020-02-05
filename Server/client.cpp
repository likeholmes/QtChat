#include "client.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QSharedPointer>
#include <QSqlError>

static QString resourceBasePath = "D:/QtProject/chatAll/Server/resource/";

Client::Client(QObject *parent) : QObject(parent)
{

}

Client::Client(QTcpSocket *socket, QObject *parent):QObject(parent)
{
    m_socket = socket;
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnect);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::nextPendingRequest);
}

void Client::disconnect(){
    //断开连接
    emit disconnected();
}

QSharedPointer<Request> Client::nextPendingRequest()
{
    //如果传来的数据是文件该怎么办，这些值该怎么处理？如果连起来处理呢
    if(m_socket->bytesAvailable()){
        QByteArray bytes = m_socket->readAll();
        //需判断bytes是否可以解析为request
        //这些数据怎么释放呢
        QSharedPointer<Request> res(new Request(bytes));
        //Request *res = new Request(bytes);
        m_request = *res;
        emit newRequest();
        return res;
    }
    return nullptr;
}

void Client::login()
{
    User user = m_request.authur();
    QSqlQuery query;
    Response rp;
    rp.setAction(Response::Login);
    rp.setResponse(Response::SUCCESS);
    QString sql = "SELECT u.id, u.name, u.describe, u.avatar, f.path path "
                  "FROM (SELECT * FROM users"
                  "WHERE account = '" + user.account() + "' AND password = '" + user.password() + "') u "
                  "LEFT JOIN files f "
                  "ON u.avatar = f.id";
    QString err;
    if(!query.exec(sql)){
        err = query.lastError().text();
        rp.setResponse(Response::FAILURE);
    }else{
        if(query.next()){  //找到用户之后；颁发某种通行证;
            //取出id,作为之后的索引返回给客户端，然后客户端再每次请求的时候发送该字段，安全暂不考虑
            int id = query.value("id").toInt();
            rp.setToken(QString().setNum(id));
            m_user.setName(query.value("name").toString());
            m_user.setAccount(user.account());
            //两个表联合查询头像地址，获取头像
            m_user.setAvatarPath(query.value("path").toString());
            m_user.loadDataFromPath();//将数据存储到user中，这一步有可能出错
            rp.setResponse(Response::SUCCESS);
            rp.setAuthur(m_user);
        }else{
            err = query.lastError().text();
        }
    }
    //需要保护，如果该套接字有其它地方要写入
    m_socket->write(rp.toByteArray());

    //检查是否有离线消息
    receiveMsg();
    emit error(err);
}

void Client::doRegister()
{
    User user = m_request.authur();
    QSqlTableModel model;
    model.setTable("users");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord newRecord;
    Response res;
    res.setAction(Response::Register);
    res.setResponse(Response::SUCCESS);
    QString err;
    //生成账号的方法过于简略
    if(!model.select()){
        err = "未成功加载用户表";
        res.setResponse(Response::FAILURE);
    }else{
        int accountIdx = 100000 + model.rowCount();
        QString account = QString(accountIdx);
        //生成账号的方法过于简略

        newRecord.setValue("name", user.name());
        newRecord.setValue("password", user.password());
        newRecord.setValue("account", account);
        user.setAccount(account);
        //检验对象不为空的方法可以这样吗
        if(!user.describe().isEmpty())
            newRecord.setValue("describe", user.describe());
        int avatarId = 1;
        if(!user.avatarData().isEmpty())
        {
            user.saveAvatar(resourceBasePath);
            //插入存放头像的数据表中，获取对应的index
            QSqlQuery query;
            QString sql = "INSERT INTO files (path) VALUES('" + user.avatarPath()
                           + "')";
            if(!query.exec(sql)){
                err = "头像插入失败";
                //response = Response::FAILURE;
            }else {
                avatarId = query.size();
            }
        }else{
            err = "用户未成功上传头像";
        }
        newRecord.setValue("avatar", avatarId);
        model.insertRecord(model.rowCount(), newRecord);
        if(!model.submitAll()){
            err = "未注册成功!";
            res.setResponse(Response::FAILURE);
        }else{//获取新插入的记录的id
            QString token = QString().setNum(model.rowCount());
            m_request.setToken(token);
            res.setToken(token);  //id//res.setToken(m_request.token());
        }
        m_user = user;
        res.setAuthur(user);
    }

    m_socket->write(res.toByteArray());
    emit error(err);
}

void Client::search()
{
    Response rp;
    rp.setAction(Response::Search);
    rp.setAuthur(m_user);
    rp.setResponse(Response::FAILURE);
    QSqlQuery query;
    QString err;
    QString sql = QString("SELECT u.id, u.name, u.describe, u.avatar, f.path path "
                          "FROM (SELECT * FROM users"
                          "WHERE account LIKE '%1' OR name LIKE '%1') u "
                          "LEFT JOIN files f "
                          "ON u.avatar = f.id").arg("%" + m_request.searchContent() + "%");
    if(!query.exec(sql))
    {
        err = "未成功执行查找数据库操作";
    }else{
        QList<User*> users;
        while (query.next()) {
            //User *user = new User;
            QSharedPointer<User> user(new User);
            user->setName(query.value("name").toString());
            user->setAccount(query.value("account").toString());
            if(!query.value("describe").isNull())
                user->setDescribe(query.value("describe").toString());
            //从头像表中查找头像数据
            user->setAvatarPath(query.value("path").toString());
            user->loadDataFromPath();
            users << user.get();
        }
        rp.setSearchContent(users);
        //释放users的内存
        rp.setResponse(Response::SUCCESS);
    }
    m_socket->write(rp.toByteArray());
    emit error(err);
}

void Client::addFriend()
{
    QString fri_account = m_request.addContent();
    Response rp;
    //rp.setAuthur(m_user);
    rp.setAction(Response::Add);
    rp.setResponse(Response::SUCCESS);
    //获取要添加的朋友的id
    //int fri_id = fri_account.toInt() - 100000 + 1;
    int fri_id = getId(fri_account);
    //这里简单地进行减法运算

    //获取用户的id
    //int user_id = m_request.token().toInt();
    int user_id = getId(m_user.account());
    //获取用户id

    //最好判断一下运行成功了没
    QSqlQuery query;
    QString err;
    if(!query.exec(QString("INSERT INTO contacts (user1, user2) VALUES(%1, %2)").
               arg(user_id).arg(fri_id))){
        err = "未成功添加至联系人";
        rp.setResponse(Response::FAILURE);
    }else{
        if(!query.exec(QString("INSERT INTO contacts (user1, user2) VALUES(%1, %2)").
                       arg(fri_id).arg(user_id))){
            err = "未成功添加至联系人";
            rp.setResponse(Response::FAILURE);
        }
    }
    m_socket->write(rp.toByteArray());
    emit error(err);
}

void Client::sendMsg()
{
    Response rp;
    rp.setAction(Response::Send);
    //rp.setAuthur(m_user);
    rp.setResponse(Response::SUCCESS);
    Message msg = m_request.msgContent();
    QSqlTableModel model;
    model.setTable("conversations");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString err;
    if(!model.select()){
        err = "未成功加载对话表";
        rp.setResponse(Response::FAILURE);
    }else{
        int fileIndex = model.rowCount() + 1;
        msg.setFileIndex(fileIndex);
        QString content = msg.textMsg();
        if(msg.type() != Message::Text){
            if(msg.fileSize() < 65536) {
                //fileIndex和filename和filePath的设置问题我已经晕了
                msg.saveSmallFile(resourceBasePath, Message::Server);
                //总之通过某种操作已经将小文件保存到了服务器上
                //并将文件路径保存到了存储文件的表中
                //如何将fileIndex传给另一个客户端呢
            }else {
                upload(msg);
                //加入到数据库中
            }
            QSqlQuery query;
            if(!query.exec(QString("INSERT INTO IGNORE files (fileIndex, path)"
                                  "VALUES(%1, '%2')").arg(fileIndex).arg(msg.filePath()))){
                err = "未成功将文件数据插入文件表中";
                rp.setResponse(Response::FAILURE);
            }
            content = QString().setNum(fileIndex);
        }
        //加到数据库中
        QSqlRecord newRecord;
        newRecord.setValue("authur", getId(msg.authur()));
        newRecord.setValue("recipient", getId(msg.recipient()));
        newRecord.setValue("type", msg.type());
        newRecord.setValue("time", msg.timeStamp());
        newRecord.setValue("content", content);
        if(!model.insertRecord(model.rowCount(), newRecord)){
            err = "未成功将消息存到对话表中";
            rp.setResponse(Response::FAILURE);
        }else if(!model.submitAll()){
            err = "未成功将消息存到对话表中";
            rp.setResponse(Response::FAILURE);
        }
    }
    m_socket->write(rp.toByteArray());
    emit error(err);
}

void Client::receiveMsg()
{
    QString err;
    QSqlTableModel model;
    model.setTable("conversations");
    model.setFilter(QString("received = 0 AND (recipient = %1 OR authur = %1)")
                    .arg(getId(m_user.account()))); //这样直接传值很容易出错
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    if(model.select()){
        for(int i = 1; i <= model.rowCount(); i++){
            Response rp;
            rp.setAction(Response::Receive);
            Message msg;
            QSqlRecord aRecord = model.record(i);
            msg.setType(Message::Type(aRecord.value("type").toInt()));
            msg.setAuthur(getAccount(aRecord.value("authur").toInt()));
            msg.setRecipient(getAccount(aRecord.value("recipient").toInt()));
            msg.setTimeStamp(aRecord.value("time").toString());
            if(msg.type() == Message::Text){
                msg.setTextMsg(aRecord.value("content").toString());
            }else{
                int index = aRecord.value("content").toInt();
                QString path = filePath(index);
                msg.setFilePath(path);
                msg.setFileIndex(index);
                msg.dealFile();
                //不用发送大文件
            }
            rp.setMsgContent(msg);
            rp.setResponse(Response::SUCCESS);
            m_socket->write(rp.toByteArray());
            //修改当前记录的标志位
            aRecord.setValue("received", 1);
            aRecord.setGenerated("received", false);
            if(!model.setRecord(i, aRecord)){
                err = "未成功更新接收消息的标志位";
            }
            m_socket->write(rp.toByteArray());
        }
        if(!model.submitAll())
            err = "接收信息大的修改未提交成功";
    }else{
        err = "对话表加载失败";
    }
    emit error(err);
}

void Client::download(int index)
{
    Response rp;
    rp.setAction(Response::Download);
    rp.setResponse(Response::SUCCESS);
    Message msg;
    //还是需要标明数据的结尾？
    //为什么：可能获取的数据是另一个请求的数据；
    QFile file(filePath(index));
    if(file.exists()){
        msg.setType(Message::File); //不管了，假设大文件通通是file
        msg.setFileIndex(index);
        msg.setFileName(file.fileName());
        msg.setFileSize(file.size());
        rp.setMsgContent(msg);

        file.open(QIODevice::ReadOnly);
        qint64 chunk = 65536;
        const char* end = "file is end!";
        //暂时不加结尾标识
        //注意加锁
        m_socket->write(rp.toByteArray());

        while (!file.atEnd()) {
            m_socket->write(file.read(chunk));
        }

        //注意加锁
        m_socket->write(end);

        file.close();
    }else{
        rp.setResponse(Response::FAILURE);
        m_socket->write(rp.toByteArray());
    }
}

void Client::upload(Message &msg)
{
    QStringList typeMap;
    typeMap[Message::File] = "file";
    typeMap[Message::Picture] = "picture";
    QString path = resourceBasePath + typeMap.at(msg.type()) + "/" + msg.fileName(); //相对位置
    msg.setFilePath(path);
    QFile file(path);
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        //需要判断该文件是否存在
        qint64 chunk = 65536;
        const char* end = "file is end!";
        //注意加锁
        while (msg.fileSize() > chunk) {
            QByteArray bytes = m_socket->read(chunk);
            if(bytes != end){
                file.write(bytes);
            }else{
                break;
            }
        }
        //注意加锁
        file.close();
    }

}

QString Client::filePath(int fileIndex)
{
    QSqlQuery query;
    QString err;
    QString res;
    if(!query.exec("SELECT * FROM files WHERE fileIndex = " + QString().setNum(fileIndex)))
    {
        err = "执行查询文件地址失败";
    }else if(query.next()){
        res = query.value("path").toString();
    }else{
        err = "不是有效的fileIndex";
    }
    return res;
    emit error(err);
}

int Client::getId(const QString &account)
{
    QSqlQuery query;
    QString err;
    int res = 0;
    if(!query.exec("SELECT * FROM users WHERE account = " + account))
    {
        err = "执行查询用户序号失败";
    }else if(query.next()){
        res = query.value("id").toInt();
    }else{
        err = "不是有效的account";
    }
    return res;
    emit error(err);
}

QString Client::getAccount(int id)
{
    QSqlQuery query;
    QString err;
    QString res;
    if(!query.exec("SELECT * FROM users WHERE id = " + QString().setNum(id)))
    {
        err = "执行查询用户账号失败";
    }else if(query.next()){
        res = query.value("path").toString();
    }else{
        err = "不是有效的id";
    }
    return res;
    emit error(err);
}
