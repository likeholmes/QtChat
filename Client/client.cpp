#include "client.h"
#include "request.h"
#include "response.h"
#include "sqlconversationmodel.h"
#include "sqlcontactsmodel.h"

#include <QThread>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QDebug>

static const QString resourceBasePath = "D:/QtProject/chatAll/Client/resource/";

Client::Client(QObject *parent):
    QObject(parent)
{

    typeMap.insert(Message::Picture, "picture");
    typeMap.insert(Message::File, "file");
    socket = new QTcpSocket();
    socket->connectToHost("localhost", 1031);
    //connect(&socket, SIGNAL(QTcpSocket::error()), this, &Client::dealError);
    //connect(socket, &QTcpSocket::readyRead, this, &Client::responseHandle);
    //connect(&socket, &QTcpSocket::disconnected, &socket, &QTcpSocket::deleteLater);
    connect(socket, &QIODevice::readyRead, this, &Client::dealData);
    connect(this, &Client::dataCome, this, &Client::responseHandle);


    qDebug()<<"连接到服务器";
}

Client::~Client()
{
    socket->disconnectFromHost();
    socket->deleteLater();
    delete m_user;
    deleteLater();
}

void Client::dealLogin(const QString &account, const QString &password)
{
    //socket->abort();
    Request request;
    User user;
    user.setAccount(account);
    user.setPassword(password);
    request.setAction(Request::Login);
    request.setAuthur(user);

    socket->write(request.toByteArray());
}

void Client::dealRegister(User *user)
{
    //socket->abort();
    Request request;
    User newUser(*user);
    request.setAction(Request::Register);
    request.setToken(m_token);
    newUser.loadDataFromPath();
    request.setAuthur(newUser);
    socket->write(request.toByteArray());
    qDebug()<< "发出注册请求";
}

void Client::dealSearch(const QString &searchContent)
{
    //socket->abort();

    Request request;
    request.setAction(Request::Search);
    request.setToken(m_token);
    request.setAuthur(*m_user);
    request.setSearchContent(searchContent);

    socket->write(request.toByteArray());
}

void Client::dealAddFriend(const QString &account)
{
    //socket->abort();

    Request request;
    request.setAction(Request::Add);
    request.setToken(m_token);
    request.setAuthur(*m_user);
    request.setAddContent(account);

    socket->write(request.toByteArray());
}

void Client::dealSendMsg(Message *msg)
{
    //socket->abort();

    Request request;
    qint64 chunk = 65536;
    Message newMsg(*msg);
    newMsg.setTimeStamp();
    newMsg.dealFile(); //将小文件数据转成文字信息
    request.setAction(Request::Send);

    request.setMsgContent(newMsg);
    request.setAuthur(*m_user);
    request.setToken(m_token);

    socket->write(request.toByteArray()); //若发送的是一个大文件将先仅发送一个头信息
    if(newMsg.fileSize() > chunk)
        upload(newMsg.filePath());
}

void Client::dealDownload(const QString& path)
{
    //socket->abort();

    Request request;
    request.setAuthur(*m_user);
    request.setToken(m_token);
    request.setAction(Request::Download);


    QStringList list = path.split('/');
    QString base = list[list.size() - 1].split("___")[0];
    int fileIndex = base.remove(0, 4).toInt();

    request.setDownloadIndex(fileIndex);

    socket->write(request.toByteArray());
}

void Client::dealData()
{
    /*QByteArray end = QString("end").toUtf8();
    QByteArray bytes = socket->readAll();
    int byteSize = bytes.size();
    //qDebug() << "end:" << end.size();
    qDebug()<< "块大小"<<byteSize;
    if(byteSize > 0 && byteSize < 65536){
        if(bytes != end){
            m_data.append(bytes);
        }
        emit dataCome();
    }else if(byteSize == 65536){
        m_data.append(bytes);
    }*/
    qDebug() << "可获取的数据" << socket->bytesAvailable();
    QDataStream in(socket);
    bool status = false;
    QByteArray bytes;
    do{
        if(status && bytes != m_data){
            m_data = bytes;
            emit dataCome();
        }
        in.startTransaction();
        in >> bytes;
    }while(socket->bytesAvailable()&&(status = in.commitTransaction()));
    if(in.commitTransaction() && bytes != m_data){
        m_data = bytes;
        emit dataCome();
    }
}

void Client::responseHandle()
{
    QByteArray bytes = m_data;
    qDebug() << bytes.size();
    m_data.clear();
    Response res(bytes);

    if(res.isNull()){
        return;
    }
    qDebug() << "处理响应";
    switch (res.action()) {
    case Response::Login:
        if(res.response() == Response::SUCCESS){
            qDebug() << "处理登录";
            m_user = new User(res.authur());
            m_user->saveAvatar(resourceBasePath);
            m_token = res.token();
            emit loginSuccess();
        }else{
            emit loginFail();
        }
        break;
    case Response::Register:
        if(res.response() == Response::SUCCESS){
            qDebug()<<"处理注册";
            m_user = new User(res.authur());
            m_user->saveAvatar(resourceBasePath);
            m_token = res.token();
            emit registerSuccess();
        }else{
            emit registerFail();
        }
        break;
    case Response::Search:
        if(res.response() == Response::SUCCESS){
            qDebug()<<"处理搜索";
            m_searchContent = res.searchResult();
            qDebug()<<"搜索结果"<<m_searchContent;
            emit searchSuccess();
        }else{
            emit searchFail();
        }
        break;
    case Response::Send:
        if(res.response() == Response::SUCCESS){
            qDebug()<<"处理发送";
            emit sendSuccess();
        }else{
            emit sendFail();
        }
        break;
    case Response::Add:
        if(res.response() == Response::SUCCESS) {
            qDebug()<<"处理添加";
            emit addSuccess();
        }else{
            emit addFail();
        }
        break;
    case Response::Receive:
        if(res.response() == Response::SUCCESS) {
            //将消息添加到本地聊天记录数据库中
            //若收到的为文件则设置为存储地址，将存储地址保存到数据库中
            //m_msg = new Message(res.msgContent());
            //m_msg->saveSmallFile(resourceBasePath, Message::Client);
            //处理接收到的小文件数据，应从文字信息转成小数据
            //用一个列表存储
            //直接加到数据库中...
            qDebug()<<"处理接收";
            SqlConversationModel model;
            QList<Message> list = res.msgContents();
            for(int i = 0; i < list.size(); ++i)
            {
                Message msg = list.at(i);
                qDebug() << "client:authur "<<msg.authur();
                model.sendMessage(&msg);
            }
            emit msgReceived(); //新加的消息会有红点显示
        }else{

        }
        break;
    case Response::Download:
        if(res.response() == Response::SUCCESS) {
            qDebug()<<"处理下载";
            //用户点击每一条消息的下载按钮，向服务器请求对应的文件序号
            //当服务器返回下载数据时，首先应该返回下载数据的大小和文件名（包括文件格式）
            //然后再主动读取二进制数据就像upload一样
            //根据文件名和格式生成本地地址，并写入数据库中
            download(res.msgContent());
            emit downloadSuccess();
        }
        break;
    case Response::Accept:
        if(res.response() == Response::SUCCESS){
            qDebug()<<"处理接受";
            SqlContactsModel model;
            QList<User> list = res.acceptContent();
            for(int i = 0; i < list.size(); ++i)
            {
                User user = list.at(i);
                user.saveAvatar(resourceBasePath);
                model.addFriend(&user);
            }
            emit acceptSuccess();
        }
    default:
        break;
    }
    //socket->flush();
}

QByteArray Client::rawData(const QString &path)
{

    QByteArray bytes;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        //出现错误
    }
    bytes = file.readAll();
    file.close();
    return bytes;
}

void Client::upload(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    qint64 chunk = 65536;
    const char* end = "file is end!";
    //暂时不加结尾标识
    //注意加锁

    while (!file.atEnd()) {
        socket->write(file.read(chunk));
    }

    //注意加锁
    socket->write(end);

    file.close();
}

void Client::download(const Message &msg)
{
    //socket->flush();
    QString path = resourceBasePath + typeMap.at(msg.type()) + "/" + msg.fileName();
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    //需要判断该文件是否存在
    qint64 chunk = 65536;
    const char* end = "file is end!";
    //注意加锁
    while (msg.fileSize() > chunk) {
        QByteArray bytes = socket->read(chunk);
        if(bytes != end){
            file.write(bytes);
        }else{
            break;
        }
    }
    //注意加锁
    file.close();
}

QQmlListProperty<User> Client::searchContent() {
    //m_searchContent
    return {this, this, &Client::appendUser, &Client::userCount, &Client::searchUser, &Client::clearUsers};
}

void Client::appendUser(User *user){
    m_searchContent.append(user);
}

int Client::userCount() const{
    return m_searchContent.count();
}

User* Client::searchUser(int idx) const{
    return m_searchContent.at(idx);
}

void Client::clearUsers(){
    m_searchContent.clear();
}

void Client::appendUser(QQmlListProperty<User> *list, User *user)
{
    reinterpret_cast<Client*>(list->data)->appendUser(user);
}

void Client::clearUsers(QQmlListProperty<User> *list)
{
    reinterpret_cast<Client*>(list->data)->clearUsers();
}

User* Client::searchUser(QQmlListProperty<User> *list, int idx)
{
    return reinterpret_cast<Client*>(list->data)->searchUser(idx);
}

int Client::userCount(QQmlListProperty<User> *list)
{
    return reinterpret_cast<Client*>(list->data)->userCount();
}
