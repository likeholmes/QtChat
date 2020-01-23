#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "myserver.h"

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

private slots:
    void on_start_pushButton_clicked();

    void on_close_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    MyServer server;
};
#endif // MAINWINDOW_H
