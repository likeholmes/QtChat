#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpServer>

#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void log(QString info);

    void handleRequest();

    void addClient();

    void removeClient();

    void dealError(const QString& err);

private slots:
    void on_start_pushButton_clicked();

    void on_close_pushButton_clicked();

private:
    Client *findClient(const QString &account);

    Ui::MainWindow *ui;
    QTcpServer *m_server;
    QList<Client*> m_clients;
};
#endif // MAINWINDOW_H
