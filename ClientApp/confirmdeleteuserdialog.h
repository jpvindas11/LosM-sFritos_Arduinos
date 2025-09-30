#ifndef CONFIRMDELETEUSERDIALOG_H
#define CONFIRMDELETEUSERDIALOG_H

#include <QDialog>

namespace Ui {
class confirmDeleteUserDialog;
}

class confirmDeleteUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit confirmDeleteUserDialog(QWidget *parent = nullptr);
    ~confirmDeleteUserDialog();

    void setUsername(QString user);

private:
    Ui::confirmDeleteUserDialog *ui;

    QString user;
};

#endif // CONFIRMDELETEUSERDIALOG_H
