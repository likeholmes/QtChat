#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>

#include "sqlcontactsmodel.h"
#include "sqlconversationmodel.h"
#include "client.h"
#include "message.h"
static bool connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    const QString dataName = "client";
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName(dataName);
    if(!db.open()){
        qFatal("Cannot open database: %s", qPrintable(db.lastError().text()));
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Some organization");

    QGuiApplication app(argc, argv);

    qmlRegisterType<SqlContactsModel>("io.qtchat.model", 1, 0, "SqlContactsModel");
    qmlRegisterType<SqlConversationModel>("io.qtchat.model", 1, 0, "SqlConversationModel");
    qmlRegisterType<Message>("io.qtchat.mytype", 1, 0, "Message");
    qmlRegisterType<User>("io.qtchat.mytype", 1, 0, "User");

    if(connectToDatabase()){
        return 1;
    }

    Client client;
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.rootContext()->setContextProperty("client", &client);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
