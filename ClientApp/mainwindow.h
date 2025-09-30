#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "autenticatorqt.h"
#include "userdataqt.h"
#include "Master.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(AuthenticationServer* authServer, Master* masterServer, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    userDataQt currentData;
    autenticatorQt autenticate;
    AuthenticationServer* authServer;
    Master* masterServer;

};
#endif // MAINWINDOW_H
