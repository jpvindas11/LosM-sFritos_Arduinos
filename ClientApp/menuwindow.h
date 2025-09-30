#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "autenticatorqt.h"
#include "usermenumanager.h"
#include "changepassdialog.h"
#include "setrankdialog.h"
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
    explicit MenuWindow(AuthenticationServer* authServer,QWidget *parent = nullptr);
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

private:
    Ui::MenuWindow *ui;
    userDataQt currentUser;
    void setActiveMenu(QPushButton *activeBtn, const QString &labelText);

    void hideFuctionsForRanks(int rank);

    void hideMenuWidgets();

    AuthenticationServer* authServer;

    userMenuManager userMenu;
};

#endif // MENUWINDOW_H
