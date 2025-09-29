#include "newuserdialog.h"
#include "ui_newuserdialog.h"

newUserDialog::newUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newUserDialog)
{
    ui->setupUi(this);
}

newUserDialog::~newUserDialog()
{
    delete ui;
}

QString newUserDialog::getUsername() const {
    return ui->lineEdit_username->text();
}

QString newUserDialog::getPassword() const {
    return ui->lineEdit_password->text();
}
