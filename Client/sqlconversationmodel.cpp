#include "sqlconversationmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>

static const char * conversationTableName = "conversation";
static void createTable()
{
    /*if(QSqlDatabase::database().tables().contains(conversationTableName)){
        return;
    }*/

    QSqlQuery query;
    query.exec("DROP TABLE IF EXISTS 'conversation'");
    if (!query.exec("CREATE TABLE 'conversation' ("
                    "'id' INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "'type' TEXT NOT NULL,"
                    "'content' TEXT NOT NULL,"
                    "'sender' TEXT NOT NULL,"
                    "'receiver' TEXT NOT NULL,"
                    "'time' TEXT NOT NULL, "
                    "FOREIGN KEY('receiver') REFERENCES contacts ( account )"
                    ")"))
    {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    query.exec("INSERT INTO contacts (type, content, sender, receiver, time)"
               "VALUES('text', 'hello', 'me', '1037', '2020-01-07T14:36:06')");
    query.exec("INSERT INTO contacts (type, content, sender, receiver, time)"
               "VALUES('text', 'hi', '1037', 'me', '2020-01-07T14:36:06')");
}

SqlConversationModel::SqlConversationModel(QObject * parent):
    QSqlTableModel(parent)
{
    createTable();
    setTable(conversationTableName);
    setSort(5, Qt::DescendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

QString SqlConversationModel::recipient() const
{
    return m_recipient;
}

void SqlConversationModel::setRecipient(const QString &recipient)
{
    if (m_recipient == recipient)
        return;

    m_recipient = recipient;
    //目前只适用于非群聊
    QString filter = QString("sender='%1' OR receiver='%1'").arg(recipient);
    setFilter(filter);
    select();

    emit recipientChanged();
}

QHash<int, QByteArray> SqlConversationModel::roleNames() const
{
    QHash<int, QByteArray> names;
    for (int i = 0; i < record().count(); i++){
        names[Qt::UserRole + i] = record().fieldName(i).toUtf8();
    }
    return names;
}

QVariant SqlConversationModel::data(const QModelIndex &idx, int role) const
{
    if(!idx.isValid())
        return QSqlTableModel::data(idx, role);

    const QSqlRecord result = record(idx.row());
    return result.value(role - Qt::UserRole);
}

void SqlConversationModel::sendMessage(const QString &type, const QString &content,
                                       const QString &sender, const QString &receiver)
{
    const QString timeStamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlRecord newRecord = record();
    newRecord.setValue("type", type);
    newRecord.setValue("content", content);
    newRecord.setValue("sender", sender);
    newRecord.setValue("receiver", receiver);
    newRecord.setValue("time", timeStamp);

    if(!insertRecord(rowCount(), newRecord)){
        qWarning() << "Failed to send message:" << lastError().text();
        return;
    }

    submitAll();
}

