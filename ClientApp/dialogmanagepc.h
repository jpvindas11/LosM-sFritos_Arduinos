#ifndef DIALOGMANAGEPC_H
#define DIALOGMANAGEPC_H

#include <QDialog>
#include <QMessageBox>
#include "ssh.hpp"

namespace Ui {
class DialogManagePC;
}

class DialogManagePC : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManagePC(QWidget *parent = nullptr);
    ~DialogManagePC();
    void setPCIndex(int index);

private slots:
    void on_on_button_clicked();
    void on_off_button_clicked();
    void on_exit_button_clicked();

private:
    Ui::DialogManagePC *ui;
    int index;

    // Variables de conexión SSH
    std::string targetPC;
    const std::string labUser = "lab-3-5";
    const std::string labPass = "Cata2960!";

    // Función helper para mostrar mensajes con estilo
    void showStyledMessage(const QString& title, const QString& message, bool isError = false);
};

#endif // DIALOGMANAGEPC_H
