#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "usermenumanager.h"
#include "datamenumanager.h"
#include "changepassdialog.h"
#include "arduinomenumanager.h"
#include "setrankdialog.h"
#include "Socket.hpp"
#include "User.hpp"
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
    explicit MenuWindow(QWidget *parent = nullptr);
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

    void on_arduino_consultar_Clicked();

    void on_arduino_turn_clicked();

private:
    Ui::MenuWindow *ui;
    userDataQt currentUser;
    void setActiveMenu(QPushButton *activeBtn, const QString &labelText);

    void hideFuctionsForRanks(int rank);

    void hideMenuWidgets();

    void askForUsers();

    userMenuManager userMenu;

    arduinoMenuManager arduinoMenu;

    dataMenuManager dataMenu;

    // Saved users
    std::vector<UserInfo> users;
};

#endif // MENUWINDOW_H
