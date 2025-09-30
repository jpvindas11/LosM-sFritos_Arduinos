#ifndef CHANGEPASSDIALOG_H
#define CHANGEPASSDIALOG_H

#include <QDialog>

namespace Ui {
class changePassDialog;
}

class changePassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit changePassDialog(QWidget *parent = nullptr);
    ~changePassDialog();

    QString getFirstPass() const;
    QString getSecondPass() const;

    void setUsername(QString user);

private:
    Ui::changePassDialog *ui;
};

#endif // CHANGEPASSDIALOG_H
