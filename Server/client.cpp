#include "client.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QScopedPointer>
#include <QSqlError>

static QString resourceBasePath = "D:/QtProject/chatAll/Server/resource/";

Client::Client(QObject *parent) : QObject(parent)
{

}

Client::Client(QTcpSocket *socket, QObject *parent):QObject(parent)
{
    m_socket = socket;
    in.setDevice(m_socket);
    end = QString("end").toUtf8();
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnect);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::nextPendingRequest);
}

void Client::disconnect(){
    //断开连接
    emit disconnected();
}

void Client::nextPendingRequest()
{
    qDebug() <<"nextPendingRequest---";
    //如果传来的数据是文件该怎么办，这些值该怎么处理？如果连起来处理呢
    bool status = false;
    QByteArray bytes;
    do{
        if(status && bytes != m_data){
            m_data = bytes;
            m_request = m_data;
            m_data.clear();
            emit newRequest();
        }
        in.startTransaction();
        in >> bytes;
        qDebug() << "传输的数据大小" << bytes.size();

    }while(m_socket->bytesAvailable()&&(status = in.commitTransaction()));
    if(in.commitTransaction() && bytes != m_data){
        m_data = bytes;
        m_request = m_data;
        m_data.clear();
        emit newRequest();
    }
    qDebug() <<"nextPendingRequest+++";
}

void Client::login()
{
    User user = m_request.authur();
    QSqlQuery query;
    Response rp;
    rp.setAction(Response::Login);
    rp.setResponse(Response::SUCCESS);
    QString sql = "SELECT u.id, u.name, u.describe, u.avatar, f.path path "
                  "FROM (SELECT * FROM users "
                  "WHERE account = '" + user.account() + "' AND password = '" + user.password() + "') u "
                  "LEFT JOIN files f "
                  "ON u.avatar = f.id";
    QString err;
    bool loginsuccess = false;
    if(!query.exec(sql)){
        err = "执行查询用户表出错";
        qDebug()<<query.lastError();
        rp.setResponse(Response::FAILURE);
    }else{
        if(query.next()){  //找到用户之后；颁发某种通行证;
            //取出id,作为之后的索引返回给客户端，然后客户端再每次请求的时候发送该字段，安全暂不考虑
            int id = query.value("id").toInt();   
            m_token = QString().setNum(id);
            rp.setToken(m_token);
            user.setName(query.value("name").toString());
            user.setAccount(user.account());
            //两个表联合查询头像地址，获取头像
            qDebug()<<"LOGIN";
            user.setAvatarPath(query.value("path").toString());
            qDebug()<<"LOGIN";
            user.loadDataFromPath();//将数据存储到user中，这一步有可能出错
            rp.setResponse(Response::SUCCESS);
            rp.setAuthur(user);
            m_user = user;
            loginsuccess = true;
        }else{
            err = "没有该用户 "+user.account();
            rp.setResponse(Response::FAILURE);
        }
    }
    //需要保护，如果该套接字有其它地方要写入
    writeSocket(rp.toByteArray());

    //检查是否有离线消息
    if(loginsuccess){
        err = user.account() + "登录成功";
        //qDebug()<<"接收完数据后";
        accept();
        receiveMsg();
    }
    if(!err.isEmpty())
        emit error(err);
}

void Client::doRegister()
{
    User user = m_request.authur();
    QSqlTableModel model;
    model.setTable("users");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord newRecord = model.record();
    Response res;
    res.setAction(Response::Register);
    res.setResponse(Response::SUCCESS);
    QString err;
    //生成账号的方法过于简略
    if(!model.select()){
        err = model.lastError().text();
        res.setResponse(Response::FAILURE);
    }else{
        int accountIdx = 100000 + model.rowCount();
        QString account = QString().setNum(accountIdx);
        //生成账号的方法过于简略

        newRecord.setValue("name", user.name());
        newRecord.setValue("password", user.password());
        newRecord.setValue("account", account);
        newRecord.setValue("isgroup", user.isGroup());
        user.setAccount(account);
        //检验对象不为空的方法可以这样吗
        if(!user.describe().isEmpty())
            newRecord.setValue("describe", user.describe());
        int avatarId = 1;
        if(!user.avatarData().isEmpty())
        {
            user.saveAvatar(resourceBasePath);
            //插入存放头像的数据表中，获取对应的index
            int fileIndex = QDateTime::currentMSecsSinceEpoch();
            QSqlQuery query;
            qDebug()<<user.avatarPath();
            qDebug()<<fileIndex;
            QString sql = QString("INSERT INTO files (path, fileIndex, type) VALUES("
                                  "'%1', %2, 2)").arg(user.avatarPath()).arg(fileIndex);
            if(!query.exec(sql)){
                qDebug() << query.lastError();
                err = "头像插入失败";
                return;
                //response = Response::FAILURE;
            }else {
                avatarId = getFileId(fileIndex);
            }
        }else{
            qDebug() << "用户未上传头像";
            user.setAvatarPath(getPath(0));
            user.loadDataFromPath();
        }
        newRecord.setValue("avatar", avatarId);

        if(!model.insertRecord(model.rowCount(), newRecord))
        {
            err = "未成功插入记录";
            qDebug() << model.lastError();
            res.setResponse(Response::FAILURE);
        }else{
            if(!model.submitAll()){
                err = "未成功提交记录";
                qDebug() << model.lastError();
                res.setResponse(Response::FAILURE);
            }else{//获取新插入的记录的id
                QString token = QString().setNum(model.rowCount());
                m_token = token;
                m_request.setToken(token);
                res.setToken(token);  //id//res.setToken(m_request.token());
            }
            m_user = user;
            res.setAuthur(user);
        }
    }
    err = user.account() + "注册成功";
    writeSocket(res.toByteArray());
    if(!err.isEmpty())
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
    QString sql = QString("SELECT u.*, f.path path "
                          "FROM (SELECT * FROM users "
                          "WHERE account LIKE '%1' OR name LIKE '%1') u "
                          "LEFT JOIN files f "
                          "ON u.avatar = f.id").arg("%" + m_request.searchContent() + "%");
    if(!query.exec(sql))
    {
        qDebug()<<query.lastError();
        err = "未成功执行查找数据库操作";
    }else{
        QList<User*> users;
        while (query.next()) {
            //User *user = new User;
            User* user(new User);
            user->setName(query.value("name").toString());
            user->setAccount(query.value("account").toString());
            if(!query.value("describe").isNull())
                user->setDescribe(query.value("describe").toString());
            //从头像表中查找头像数据
            user->setAvatarPath(query.value("path").toString());
            user->loadDataFromPath();
            users << user;
        }
        rp.setSearchContent(users);
        //释放users的内存
        for(User* i : users){
            delete i;
        }
        rp.setResponse(Response::SUCCESS);
    }
    writeSocket(rp.toByteArray());
    if(!err.isEmpty())
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
    writeSocket(rp.toByteArray());
    if(!err.isEmpty())
        emit error(err);
    if(rp.response() == Response::SUCCESS){
        emit add(fri_account);
    }
}

void Client::sendMsg()
{
    qDebug() << "sendMsg---";
    Response rp;
    rp.setAction(Response::Send);
    //rp.setAuthur(m_user);
    rp.setResponse(Response::SUCCESS);
    qDebug() << "转成msg---";
    Message msg = m_request.msgContent();
    qDebug() << "filePath : " << msg.filePath();
    qDebug() << "转成msg+++";
    QSqlTableModel model;
    model.setTable("conversations");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QString err;
    if(!model.select()){
        err = "未成功加载对话表";
        rp.setResponse(Response::FAILURE);
    }else{
        int fileIndex = QDateTime::currentMSecsSinceEpoch();
        msg.setFileIndex(fileIndex);
        qDebug() << "获取content---";
        QString content = msg.textMsg();
        qDebug() << "获取content+++";
        if(msg.type() != Message::Text){
            if(msg.fileSize() < 655360) {
                qDebug() <<"小于655360---";
                //fileIndex和filename和filePath的设置问题我已经晕了
                msg.saveSmallFile(resourceBasePath, Message::Server);
                //总之通过某种操作已经将小文件保存到了服务器上
                //并将文件路径保存到了存储文件的表中
                //如何将fileIndex传给另一个客户端呢
                qDebug() <<"小于655360+++";
            }else {
                qDebug() <<"大于655360---";
                upload(msg);
                //加入到数据库中
                qDebug() <<"大于655360+++";
            }
            QSqlQuery query;
            if(!query.exec(QString("INSERT INTO files (fileIndex, path, type)"
                                  "VALUES(%1, '%2', %3)").arg(fileIndex)
                           .arg(msg.filePath()).arg(msg.type()))){
                err = "未成功将文件数据插入文件表中";
                qDebug() <<query.lastError();
                rp.setResponse(Response::FAILURE);
            }
            content = QString().setNum(fileIndex);
        }
        //加到数据库中
        QSqlRecord newRecord = model.record();
        newRecord.setValue("authur", getId(msg.authur()));
        newRecord.setValue("recipient", getId(msg.recipient()));
        newRecord.setValue("type", msg.type());
        newRecord.setValue("time", msg.timeStamp());
        newRecord.setValue("content", content);
        newRecord.setValue("received", 0);
        if(!model.insertRecord(model.rowCount(), newRecord)){
            err = "未成功将消息插入对话表中";
            qDebug()<<model.lastError();
            rp.setResponse(Response::FAILURE);
        }else if(!model.submitAll()){
            err = "未成功将消息提交到对话表中";
            qDebug()<<model.lastError();
            rp.setResponse(Response::FAILURE);
        }
    }
    writeSocket(rp.toByteArray());
    if(!err.isEmpty())
        emit error(err);
    if(rp.response() == Response::SUCCESS){
        emit send(msg.recipient());
    }
    qDebug() << "sendMsg+++";
}

void Client::receiveMsg()
{
    qDebug() << "receive ---";
    QString err;
    QSqlTableModel model;
    model.setTable("conversations");
    model.setFilter(QString("received = 0 AND (recipient = %1 OR authur = %1)")
                    .arg(getId(m_user.account()))); //这样直接传值很容易出错
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QList<Message> list;
    Response rp;
    rp.setAction(Response::Receive);
    rp.setResponse(Response::FAILURE);
    if(model.select()){
        for(int i = 0; i < model.rowCount(); ++i){
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
                QString path = getPath(index);
                msg.setFilePath(path);
                msg.setFileIndex(index);
                msg.dealFile();
                //不用发送大文件
            }
            aRecord.setValue("received", 1);
            if(!model.setRecord(i, aRecord)){
                err = "未成功更新接收消息的标志位";
            }else {
                list << msg;
            }
        }
        if(!model.submitAll())
            err = "接收信息大的修改未提交成功";
        else{
            rp.setResponse(Response::SUCCESS);
            rp.setMsgContents(list);
        }
    }else{
        err = "对话表加载失败";
    }
    qDebug() << rp.toByteArray().size();
    writeSocket(rp.toByteArray());
    if(!err.isEmpty())
        emit error(err);
    qDebug() << "receive +++";
}

void Client::download(int index)
{
    Response rp;
    rp.setAction(Response::Download);
    rp.setResponse(Response::SUCCESS);
    Message msg;
    //还是需要标明数据的结尾？
    //为什么：可能获取的数据是另一个请求的数据；
    QFile file(getPath(index));
    if(file.exists()){
        msg.setType(getType(index));
        msg.setFileIndex(index);
        msg.setFileName(file.fileName());
        msg.setFileSize(file.size());
        rp.setMsgContent(msg);

        file.open(QIODevice::ReadOnly);
        qint64 chunk = 65536;
        //暂时不加结尾标识
        //注意加锁
        writeSocket(rp.toByteArray());

        while (!file.atEnd()) {
            m_socket->write(file.read(chunk));
        }

        //注意加锁

        file.close();
    }else{
        rp.setResponse(Response::FAILURE);
        writeSocket(rp.toByteArray());
    }
}

void Client::upload(Message &msg)
{
    qDebug() << "update---";
    QStringList typeMap;
    typeMap.push_back("none");
    typeMap.push_back("text");
    typeMap.push_back("picture");
    typeMap.push_back("file");
    QString path = resourceBasePath + typeMap.at(msg.type()) + "/" + msg.fileName();
    QFile file(path);
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        //需要判断该文件是否存在
        qint64 toRead = msg.fileSize();
        qDebug() << "filesize = " << toRead;
        //注意加锁
        while (toRead) {
            QByteArray bytes = m_socket->readAll();
            file.write(bytes);
            qDebug() << "一次读取的数据为"<<bytes.size();
            toRead -= bytes.size();
        }
        //注意加锁
        file.close();
    }
    qDebug() << "update+++";
}

QString Client::getPath(int fileIndex)
{
    qDebug() << "getPath---";
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
    if(!err.isEmpty())
        emit error(err);
    qDebug() << "getPath+++";
    return res;
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

    if(!err.isEmpty())
        emit error(err);
    return res;
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
        res = query.value("account").toString();
    }else{
        err = "不是有效的id";
    }

    if(!err.isEmpty())
        emit error(err);
    return res;
}

Message::Type Client::getType(int fileIndex)
{
    QSqlQuery query;
    QString err;
    Message::Type res = Message::File;
    if(!query.exec("SELECT * FROM files WHERE fileIndex = " + QString().setNum(fileIndex)))
    {
        err = "执行查询文件类型失败";
    }else if(query.next()){
        res = Message::Type(query.value("type").toInt());
    }else{
        err = "不是有效的fileIndex";
    }

    if(!err.isEmpty())
        emit error(err);
    return res;
}

int Client::getFileId(int fileIndex)
{
    QSqlQuery query;
    QString err;
    int res = 1;
    if(!query.exec("SELECT * FROM files WHERE fileIndex = " + QString().setNum(fileIndex)))
    {
        err = "执行查询文件id失败";
    }else if(query.next()){
        res = query.value("id").toInt();
    }else{
        err = "不是有效的fileIndex";
    }

    if(!err.isEmpty())
        emit error(err);
    return res;
}

void Client::writeSocket(const QByteArray &bytes)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    //m_socket->write(bytes);
    out << bytes;
    /*if(bytes.size() > 0 && bytes.size() % 65536 == 0){
        //m_socket->write(end);
        out << end;
        //qDebug()<<"end";
    }*/
    m_socket->write(data);
    //m_socket->flush();
}

void Client::accept()
{
    qDebug()<<"accept:"<<m_token;
    QString sql = "SELECT u.name, u.describe, u.account, u.isgroup , u.path "
                  "FROM (SELECT * FROM contacts WHERE user2 = " + m_token +
                  ") c "
                  "LEFT JOIN (SELECT u.id, u.name, u.account, u.describe, u.isgroup, f.path "
                  "FROM users u JOIN files f "
                  "ON u.avatar = f.id) u "
                  "ON c.user1 = u.id";
    QString err;
    QSqlQuery query;
    Response rp;
    rp.setAuthur(m_user);
    rp.setAction(Response::Accept);
    rp.setResponse(Response::SUCCESS);
    QList<User> list;
    if(!query.exec(sql)){
        qDebug() << query.lastError();
        err = "接受未成功执行查询语句";
        rp.setResponse(Response::FAILURE);
    }else{
        while(query.next()){
            User user;
            user.setName(query.value("name").toString());
            qDebug()<<"accept:接收的朋友名称："<<user.name();
            user.setAccount(query.value("account").toString());
            user.setIsGroup(query.value("isgroup").toInt());
            user.setDescribe(query.value("describe").toString());
            //qDebug()<<"accept";
            user.setAvatarPath(query.value("path").toString());
            //qDebug()<<"accept";
            user.loadDataFromPath();
            list << user;
        }
        rp.setAcceptContent(list);
    }
    qDebug() << rp.toByteArray().size();
    writeSocket(rp.toByteArray());
    if(!err.isEmpty())
        emit error(err);
}
