#ifndef SETRANKDIALOG_H
#define SETRANKDIALOG_H

#include <QDialog>

namespace Ui {
class setRankDialog;
}

class setRankDialog : public QDialog
{
    Q_OBJECT

public:
    explicit setRankDialog(QWidget *parent = nullptr);
    ~setRankDialog();

    char getRank();
    void setUsername(QString user);

private:
    Ui::setRankDialog *ui;
};

#endif // SETRANKDIALOG_H
