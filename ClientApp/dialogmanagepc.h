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
    std::string targetPC;

    std::string labUser = "lab3-5";
    std::string labPass = "Cata2960!";

};

#endif // DIALOGMANAGEPC_H
