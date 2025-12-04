#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "usermenumanager.h"
#include "datamenumanager.h"
#include "changepassdialog.h"
#include "arduinomenumanager.h"
#include "setrankdialog.h"
#include "dialogmanagepc.h"
#include "Socket.hpp"
#include "userdataqt.h"
#include "User.hpp"
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QTimer>

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

    void on_b_cerrarSesion_clicked();

    void on_user_list_itemClicked(QListWidgetItem *item);
    void on_user_list_itemDoubleClicked(QListWidgetItem *item);

    void on_user_add_clicked();

    void on_user_delete_clicked();

    void on_user_change_pass_clicked();

    void on_user_change_rank_clicked();

    void on_b_nodos_clicked();

    void on_b_consulta_clicked();

    void on_data_list_itemClicked(QListWidgetItem *item);

    void updateSensorDataAutomatically();

    void on_b_compus_clicked();

    void on_compus_list_itemClicked(QListWidgetItem *item);

    void on_pc1_button_clicked();

    void on_pc2_button_clicked();

    void on_pc3_button_clicked();

    void on_pc4_button_clicked();

private:
    Ui::MenuWindow *ui;
    userDataQt currentUser;
    void setActiveMenu(QPushButton *activeBtn, const QString &labelText, int pageIndex);

    void hideFuctionsForRanks(int rank);

    void hideMenuWidgets();

    void askForUsers();

    void askForSensorData();

    void askForUserLogs(const std::string& username);

    void askForSensorLogs(const std::string& sensorIP, const std::string& sensorType);

    void askForServerStatus();

    userMenuManager userMenu;

    dataMenuManager dataMenu;

    std::vector<sensorRecentData> sensorsData;

    // Saved users
    std::vector<UserInfo> users;

    QTimer *sensorUpdateTimer;
};

#endif // MENUWINDOW_H
