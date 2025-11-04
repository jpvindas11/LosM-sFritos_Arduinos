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

char newUserDialog::getRank() {
    if (ui->radioButton_c->isChecked()) { return 'c'; }
    else if (ui->radioButton_h->isChecked()) { return 'h'; }
    else if (ui->radioButton_o->isChecked()) { return 'o'; }
    else if (ui->radioButton_u->isChecked()) { return 'u'; }
    return '0';
}
