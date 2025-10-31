#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "userdataqt.h"
#include "Socket.hpp"
#include "Messages.hpp"
#include "IPConstants.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_ip_clicked();

private:
    Ui::MainWindow *ui;
    userDataQt currentData;
    bool isConnected;
};
#endif // MAINWINDOW_H
