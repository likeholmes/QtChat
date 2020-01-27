#ifndef SQLCONVERSATIONMODEL_H
#define SQLCONVERSATIONMODEL_H

#include <QSqlTableModel>
class SqlConversationModel: public QSqlTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString recipient READ recipient WRITE setRecipient NOTIFY recipientChanged)
public:
    SqlConversationModel(QObject * parent = nullptr);

    QString recipient() const;
    void setRecipient(const QString & recipient);

    Q_INVOKABLE void sendMessage(const QString &type, const QString &content,
                                 const QString &sender, const QString &receiver);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;

signals:
    void recipientChanged();

private:
    QString m_recipient;
};

#endif // SQLCONVERSATIONMODEL_H
