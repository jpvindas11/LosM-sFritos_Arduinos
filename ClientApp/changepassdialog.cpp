#include "changepassdialog.h"
#include "ui_changepassdialog.h"

changePassDialog::changePassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changePassDialog)
{
    ui->setupUi(this);
}

changePassDialog::~changePassDialog()
{
    delete ui;
}

QString changePassDialog::getFirstPass() const {
    return ui->lineEdit_username->text();
}

QString changePassDialog::getSecondPass() const {
    return ui->lineEdit_password->text();
}

void changePassDialog::setUsername(QString name) {
    this->ui->label_userchangepass->setText(name);
}
