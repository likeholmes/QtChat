#include "client.h"
#include "request.h"
#include "response.h"

#include <QThread>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include <QUrl>
#include <QDir>

Client::Client(QObject *parent):
    QObject(parent)
{
    typeMap.insert(Message::Picture, "picture");
    typeMap.insert(Message::File, "file");
    socket = new QTcpSocket();
    socket->connectToHost("localhost", 1031);
    //connect(&socket, SIGNAL(QTcpSocket::error()), this, &Client::dealError);
    connect(socket, &QTcpSocket::readyRead, this, &Client::responseHandle);
    //connect(&socket, &QTcpSocket::disconnected, &socket, &QTcpSocket::deleteLater);
}

Client::~Client()
{
    socket->disconnectFromHost();
    socket->deleteLater();
    deleteLater();
}

void Client::dealLogin(const QString &account, const QString &password)
{
    socket->abort();
    Request request;
    User user;
    user.setAccount(account);
    user.setPassword(password);
    request.setAction(Request::Login);
    request.setAuthur(user);
    socket->write(request.toByteArray());
}

void Client::dealRegister(const User &user)
{
    socket->abort();
    Request request;
    User newUser(user);
    request.setAction(Request::Register);
    request.setToken(m_token);
    newUser.loadDataFromPath();
    request.setAuthur(newUser);
    socket->write(request.toByteArray());
}

void Client::dealSearch(const QString &searchContent)
{
    socket->abort();

    Request request;
    request.setAction(Request::Search);
    request.setToken(m_token);
    request.setAuthur(m_user);
    request.setSearchContent(searchContent);

    socket->write(request.toByteArray());
}

void Client::dealAddFriend(const QString &account)
{
    socket->abort();

    Request request;
    request.setAction(Request::Add);
    request.setToken(m_token);
    request.setAuthur(m_user);
    request.setAddContent(account);

    socket->write(request.toByteArray());
}

void Client::dealSendMsg(const Message &msg)
{
    socket->abort();

    Request request;
    qint64 chunk = 65536;
    Message newMsg(msg);
    newMsg.setTimeStamp();
    newMsg.dealFile(); //将小文件数据转成文字信息
    request.setAction(Request::Send);

    request.setMsgContent(newMsg);
    request.setAuthur(m_user);
    request.setToken(m_token);

    socket->write(request.toByteArray()); //若发送的是一个大文件将先仅发送一个头信息
    if(newMsg.fileSize() > chunk)
        upload(newMsg.filePath());
}

void Client::dealDownload(int fileIndex)
{
    socket->abort();

    Request request;
    request.setAuthur(m_user);
    request.setToken(m_token);
    request.setAction(Request::Download);
    request.setDownloadIndex(fileIndex);

    socket->write(request.toByteArray());
}

void Client::responseHandle()
{
    QByteArray bytes = socket->readAll();
    Response res(bytes);

    switch (res.action()) {
    case Response::Login:
        if(res.response() == Response::SUCCESS){
            m_user = res.authur();
            m_user.saveAvatar();
            m_token = res.token();
            emit loginSuccess();
        }else{
            emit loginFail();
        }
        break;
    case Response::Register:
        if(res.response() == Response::SUCCESS){
            m_user = res.authur();
            m_user.saveAvatar();
            m_token = res.token();
            emit registerSuccess();
        }else{
            emit registerFail();
        }
        break;
    case Response::Search:
        if(res.response() == Response::SUCCESS){
            m_searchContent = res.searchResult();
            emit searchSuccess();
        }else{
            emit searchFail();
        }
        break;
    case Response::Send:
        if(res.response() == Response::SUCCESS){
            emit sendSuccess();
        }else{
            emit sendFail();
        }
        break;
    case Response::Add:
        if(res.response() == Response::SUCCESS) {
            emit addSuccess();
        }else{
            emit addFail();
        }
        break;
    case Response::Receive:
        if(res.response() == Response::SUCCESS) {
            //将消息添加到本地聊天记录数据库中
            //若收到的为文件则设置为存储地址，将存储地址保存到数据库中
            m_msg = new Message(res.msgContent());
            //处理接收到的小文件数据，应从文字信息转成小数据
            emit msgReceived(); //新加的消息会有红点显示
        }else{

        }
        break;
    case Response::Download:
        if(res.response() == Response::SUCCESS) {
            //用户点击每一条消息的下载按钮，向服务器请求对应的文件序号
            //当服务器返回下载数据时，首先应该返回下载数据的大小和文件名（包括文件格式）
            //然后再主动读取二进制数据就像upload一样
            //根据文件名和格式生成本地地址，并写入数据库中
            download(res.msgContent());
            emit downloadSuccess();
        }
        break;
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
    //const char* end = "file is end!";
    //暂时不加结尾标识
    //注意加锁

    while (!file.atEnd()) {
        socket->write(file.read(chunk));
    }

    //注意加锁
    //socket->write(end);

    file.close();
}

void Client::download(const Message &msg)
{
    //socket->flush();
    QString path = typeMap.at(msg.type()) + "/" + msg.fileName(); //相对位置
    QFile file(QDir(path).absolutePath());
    file.open(QIODevice::WriteOnly);
    //需要判断该文件是否存在
    qint64 chunk = 65536;
    //注意加锁
    while (msg.fileSize() > chunk && !socket->atEnd()) {
        file.write(socket->read(chunk));
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

Message* Client::message() const{
    return m_msg;
}
