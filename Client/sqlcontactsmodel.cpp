#include "sqlcontactsmodel.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

static const char *contactsTableName = "contacts";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(contactsTableName))
        return;
    QSqlQuery query;
    //query.exec("DROP TABLE IF EXISTS 'contacts'");
    if (!query.exec("CREATE TABLE `contacts` ("
                        "`id` int NOT NULL AUTO_INCREMENT, "
                        "`account` TEXT NOT NULL,"
                        "`name` TEXT NOT NULL,"
                        "`avatar` TEXT NOT NULL,"
                        "`isgroup` INT NOT NULL,"
                        "`describe` TEXT NULL,"
                        "PRIMARY KEY (`id`),"
                        "UNIQUE INDEX (`account`(20)))"
                    )){
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    //query.exec("INSERT INTO contacts VALUES('1037', 'xiaohan', 'D://QtProject/chatAll/Client/image/avatar01.jpg', 0, '我就是我')");
    //query.exec("INSERT INTO contacts VALUES('1038', 'xiaoguang', 'D://QtProject/chatAll/Client/image/avatar01.jpg', 0, '你就是我')");
}

SqlContactsModel::SqlContactsModel(QObject *parent):
    QSqlTableModel(parent)
{
    createTable();
    setTable(contactsTableName);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
}

QHash<int, QByteArray> SqlContactsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::UserRole + 1] = "account";
    names[Qt::UserRole + 2] = "name";
    names[Qt::UserRole + 3] = "avatar";
    names[Qt::UserRole + 4] = "describe";
    names[Qt::UserRole + 5] = "isgroup";

    return names;
}

QVariant SqlContactsModel::data(const QModelIndex &idx, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(idx, role);

    const QSqlRecord sqlRecord = record(idx.row());
    //qDebug()<< sqlRecord.value(role - Qt::UserRole);
    return sqlRecord.value(role - Qt::UserRole);
}

void SqlContactsModel::addFriend(User *user)
{
    QSqlRecord newRecord = record();
    newRecord.setValue("account", user->account());
    newRecord.setValue("name", user->name());
    newRecord.setValue("avatar", user->avatarPath());
    newRecord.setValue("isgroup", user->isGroup());
    newRecord.setValue("describe", user->describe());
    qDebug() << "test addFriend "+user->name();
    if(!insertRecord(rowCount(), newRecord)){
        qWarning() << "Failed to add Friend:" << lastError().text();
        return;
    }

    if(!submitAll()){
        qWarning() << "Failed to submit friend" << lastError().text();
    }
}

void SqlContactsModel::flush()
{
    select();
}
