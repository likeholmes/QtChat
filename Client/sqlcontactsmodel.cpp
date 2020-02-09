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
    if (!query.exec("CREATE TABLE 'contacts' ("
                        "'account' TEXT NOT NULL ,"
                        "'name' TEXT NOT NULL ,"
                        "'avatar' TEXT NOT NULL ,"
                        "'isgroup' tinyint(4) NOT NULL ,"
                        "'describe' TEXT DEFAULT NULL ,"
                        "PRIMARY KEY ('account') )"
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
    names[Qt::UserRole] = "account";
    names[Qt::UserRole + 1] = "name";
    names[Qt::UserRole + 2] = "avatar";
    names[Qt::UserRole + 3] = "describe";
    names[Qt::UserRole + 4] = "isgroup";

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

/*void SqlContactsModel::addFriend(const QString &account, const QString &name,
                                 const QString &avatar, const int isgroup, const QString &describe)
{
    QSqlRecord newRecord = record();
    newRecord.setValue("account", account);
    newRecord.setValue("name", name);
    newRecord.setValue("avatar", avatar);
    newRecord.setValue("isgroup", isgroup);
    newRecord.setValue("describe", describe);

    if(!insertRecord(rowCount(), newRecord)){
        qWarning() << "Failed to send message:" << lastError().text();
        return;
    }

    submitAll();
}*/

void SqlContactsModel::addFriend(User *user)
{
    QSqlRecord newRecord = record();
    newRecord.setValue("account", user->account());
    newRecord.setValue("name", user->name());
    newRecord.setValue("avatar", user->avatarPath());
    newRecord.setValue("isgroup", user->isGroup());
    newRecord.setValue("describe", user->describe());
    qDebug() << "test addFriend";
    if(!insertRecord(rowCount(), newRecord)){
        qWarning() << "Failed to add Friend:" << lastError().text();
        return;
    }

    if(!submitAll()){
        qWarning() << "Failed to submit friend" << lastError().text();
    }
}
