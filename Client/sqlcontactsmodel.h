#ifndef SQLCONTACTSMODEL_H
#define SQLCONTACTSMODEL_H

#include <QSqlTableModel>

class SqlContactsModel: public QSqlTableModel
{
    Q_OBJECT
public:
    SqlContactsModel(QObject * parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;

    //Q_INVOKABLE void addFriend(const QString &account);
};

#endif // SQLCONTACTSMODEL_H
