#include "confirmdeleteuserdialog.h"
#include "ui_confirmdeleteuserdialog.h"

confirmDeleteUserDialog::confirmDeleteUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::confirmDeleteUserDialog)
{
    ui->setupUi(this);
}

confirmDeleteUserDialog::~confirmDeleteUserDialog()
{
    delete ui;
}

void confirmDeleteUserDialog::setUsername(QString name) {
    this->user = name;
    this->ui->label_warning->setText(user);
}
