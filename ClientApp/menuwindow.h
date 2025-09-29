#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "autenticatorqt.h"
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

private:
    Ui::MenuWindow *ui;
    userDataQt currentUser;
    void setActiveMenu(QPushButton *activeBtn, const QString &labelText);

    void hideFuctionsForRanks(int rank);

    AuthenticationServer* authServer;
};

#endif // MENUWINDOW_H
