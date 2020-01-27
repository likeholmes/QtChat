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

    query.exec("INSERT INTO contacts VALUES('1037', 'xiaohan', 'D://QtProject/chatAll/Client/image/avatar01.jpg', 0, '我就是我')");
    //query.exec("INSERT INTO contacts VALUES('1038', 'xiaoguang', 'D://QtProject/chatAll/Client/image/avatar01.jpg', 0, '你就是我')");
}

SqlContactsModel::SqlContactsModel(QObject *parent):
    QSqlTableModel(parent)
{
    createTable();
    setTable(contactsTableName);
    //setSort(2, Qt::DescendingOrder);
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
