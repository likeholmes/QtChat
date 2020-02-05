#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

static bool connectToDataBase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("chat");
    return db.open();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!connectToDataBase()){
        //未连接到数据库
        return 1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
