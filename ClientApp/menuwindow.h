#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "autenticatorqt.h"
#include "usermenumanager.h"
#include "changepassdialog.h"
#include "arduinomenumanager.h"
#include "setrankdialog.h"
#include "Master.hpp"
#include <QMainWindow>
#include <QPushButton>
#include <QString>

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(AuthenticationServer* authServer, Master* masterServer,QWidget *parent = nullptr);
    ~MenuWindow();

    void setCurrentUser(userDataQt user);

private slots:
    void on_b_usuarios_clicked();

    void on_b_arduinos_clicked();

    void on_b_cerrarSesion_clicked();

    void on_user_list_itemClicked(QListWidgetItem *item);

    void on_user_add_clicked();

    void on_user_delete_clicked();

    void on_user_change_pass_clicked();

    void on_user_change_rank_clicked();

    void on_b_nodos_clicked();

    void on_b_consulta_clicked();

    void on_arduino_list_itemClicked(QListWidgetItem *item);

    void on_arduino_turn_clicked();

private:
    Ui::MenuWindow *ui;
    userDataQt currentUser;
    void setActiveMenu(QPushButton *activeBtn, const QString &labelText);

    void hideFuctionsForRanks(int rank);

    void hideMenuWidgets();

    AuthenticationServer* authServer;

    Master* masterServer;

    userMenuManager userMenu;

    arduinoMenuManager arduinoMenu;
};

#endif // MENUWINDOW_H
