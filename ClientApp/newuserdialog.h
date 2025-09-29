#ifndef NEWUSERDIALOG_H
#define NEWUSERDIALOG_H

#include <QDialog>

namespace Ui {
class newUserDialog;
}

class newUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newUserDialog(QWidget *parent = nullptr);
    ~newUserDialog();

    QString getUsername() const;
    QString getPassword() const;

private:
    Ui::newUserDialog *ui;
};

#endif // NEWUSERDIALOG_H
