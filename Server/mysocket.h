#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QTcpSocket>
class MySocket: public QTcpSocket
{
    Q_OBJECT
public:
    MySocket(QObject * parent = nullptr);
private:
    //数据缓存区
    //认证信息
};

#endif // MYSOCKET_H
