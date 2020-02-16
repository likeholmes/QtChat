#include "sqlconversationmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QFileInfo>

static const char * conversationTableName = "conversation";
static void createTable()
{
    if(QSqlDatabase::database().tables().contains(conversationTableName)){
        return;
    }

    QSqlQuery query;
    //query.exec("DROP TABLE IF EXISTS 'conversation'");
    if (!query.exec("CREATE TABLE `conversation` ("
                    "`id` INT NOT NULL AUTO_INCREMENT,"
                    "`type` INT NOT NULL,"
                    "`content` TEXT NOT NULL,"
                    "`sender` TEXT NOT NULL,"
                    "`receiver` TEXT NOT NULL,"
                    "`time` TEXT NOT NULL, "
                    "PRIMARY KEY (`id`)"
                    ")"))
    {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    //if(!query.exec("INSERT INTO conversation (type, content, sender, receiver, time)"
      //         "VALUES(1, 'hello', 'me', '1037', '2020-01-07T14:36:06')"))
        //qFatal("a Failed to query database: %s", qPrintable(query.lastError().text()));
    //query.exec("INSERT INTO conversation (type, content, sender, receiver, time)"
      //         "VALUES(1, 'hi', '1037', 'me', '2020-01-07T14:36:06')");
}

SqlConversationModel::SqlConversationModel(QObject * parent):
    QSqlTableModel(parent)
{
    createTable();
    setTable(conversationTableName);
    //setSort(5, Qt::DescendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

QString SqlConversationModel::recipient() const
{
    return m_recipient;
}

void SqlConversationModel::setRecipient(const QString &recipient)
{
    /*if (m_recipient == recipient)
        return;
    */
    m_recipient = recipient;
    //目前只适用于非群聊
    const QString filterString = QString("sender='%1' OR receiver='%1'").arg(recipient);

    setFilter(filterString);

    select();

    //qDebug()<<"this is "<<record(0).value("content");
    emit recipientChanged();
}

QHash<int, QByteArray> SqlConversationModel::roleNames() const
{
    QHash<int, QByteArray> names;
    /*for (int i = 0; i < record().count(); i++){
        names[Qt::UserRole + i] = record().fieldName(i).toUtf8();
    }*/

    names[Qt::UserRole] = "id";
    names[Qt::UserRole + 1] = "type";
    names[Qt::UserRole + 2] = "content";
    names[Qt::UserRole + 3] = "sender";
    names[Qt::UserRole + 4] = "receiver";
    names[Qt::UserRole + 5] = "time";
    return names;
}

QVariant SqlConversationModel::data(const QModelIndex &idx, int role) const
{

    if(role < Qt::UserRole)
        return QSqlTableModel::data(idx, role);

    const QSqlRecord result = record(idx.row());

    return result.value(role - Qt::UserRole);
}

void SqlConversationModel::sendMessage(const Message &msg)
{
    QString timeStamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlRecord newRecord = record();
    newRecord.setValue("type", msg.type());
    newRecord.setValue("content", msg.textMsg());
    qDebug()<<"检查authur"<<msg.authur();
    newRecord.setValue("sender", msg.authur());
    qDebug()<<"检查recipient"<<msg.recipient();
    newRecord.setValue("receiver", msg.recipient());
    if(!msg.timeStamp().isEmpty())
        timeStamp = msg.timeStamp();
    newRecord.setValue("time", timeStamp);

    if(!insertRecord(rowCount(), newRecord)){
        qWarning() << "Failed to insert message:" << lastError().text();
        return;
    }

    if(!submitAll()){
        qWarning() << "Failed to submit message:" << lastError().text();
        return;
    }
}

void SqlConversationModel::sendMessage(Message *msg)
{
    sendMessage(*msg);
}

bool SqlConversationModel::fileExist(const QString &path)
{
    return QFileInfo::exists(path);
}

