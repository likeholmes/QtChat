#include "client.h"
#include "request.h"
#include "response.h"

#include <QPixmap>
#include <QBuffer>
#include <QFile>

Client::Client(QObject *parent):
    QObject(parent)
{
    socket.connectToHost("localhost", 1031);
    //connect(&socket, SIGNAL(QTcpSocket::error()), this, &Client::dealError);
    connect(&socket, &QTcpSocket::readyRead, this, &Client::responseHandle);
    //connect(&socket, &QTcpSocket::disconnected, &socket, &QTcpSocket::deleteLater);
}

void Client::dealLogin(const QString &account, const QString &password)
{
    socket.abort();
    Request request;
    request.setAction("login");
    request.setAuthur(account);
    request.setPassword(password);  //密码需要加密
    socket.write(request.toByteArray());
}

void Client::dealRegister(const QString &name, const QString &password,
                          const QString &describe, const QString &avatar)
{
    socket.abort();

    Request request;
    request.setAction("register");
    request.setToken(m_token);
    request.setPassword(password); //密码需要加密
    request.setName(name);
    request.setDescribe(describe);

    //const char * format = avatar.right(3).toStdString().c_str();
    request.setContent(rawData(avatar));

    socket.write(request.toByteArray());
}

void Client::dealSearch(const QString &user)
{
    socket.abort();

    Request request;
    request.setAction("lookup");
    request.setToken(m_token);
    request.setAuthur(m_account);
    //把要查找的账号或昵称放在type中
    request.setType(user);
    //把要查找的账号或昵称放在type中

    socket.write(request.toByteArray());
}

void Client::dealAddFriend(const QString &account)
{
    socket.abort();

    Request request;
    request.setAction("addFriend");
    request.setToken(m_token);
    request.setAuthur(m_account);

    //同上
    request.setType(account);
    //

    socket.write(request.toByteArray());
}

void Client::dealSendMsg(const QString &account, const QString &type, const QString &content)
{
    socket.abort();

    Request request;
    request.setAction("sendMsg");
    request.setType(type);
    request.setRecipient(account);

    //处理发送数据
    if(type != "text"){
        request.setContent(rawData(content));
    }else{
        request.setContent(content.toLocal8Bit());
    }
    //处理发送数据

    socket.write(request.toByteArray());
}

void Client::responseHandle()
{
    QByteArray bytes = socket.readAll();
    Response res(bytes);
    QString action = res.action();
    QString response = res.response();

    if(action == "login"){
        if(response == "success"){
            m_account = res.authur();
            m_avatar = saveData(res.content());
            m_name = res.name();
            m_describe = res.describe();
            m_token = res.token();
            emit loginSuccess();
        }else{
            emit loginFail();
        }
    }else if(action == "register"){
        if(response == "success"){
            m_account = res.authur();
            m_avatar = saveData(res.content());
            m_name = res.name();
            m_describe = res.describe();
            m_token = res.token();
            emit registerSuccess();
        }else{
            emit registerFail();
        }
    }else if(action == "lookup"){
        if(response == "success"){
            //瓶颈为：如何表达列表形式的用户信息
            emit searchSuccess();
        }else{
            emit searchFail();
        }
    }else if(action == "addFriend"){
        //添加到数据库
        if(response == "success"){
            //接收转换为User对象
            //并添加到数据库？问题是更新了数据库它能实时显示在界面上吗？
            emit addSuccess();
        }else{
            emit addFail();
        }
    }else if(action == "sendMsg"){


    }else if(action == "receiveMsg"){

    }
}

QByteArray Client::rawData(const QString &path)
{

    QByteArray bytes;
    QFile file(path);
    //计算公式可对？？？
    file.setFileName(path.right(path.length() - path.lastIndexOf('/')));
    if(!file.open(QIODevice::ReadOnly)){
        //出现错误
    }
    bytes = file.readAll();
    return bytes;
}
