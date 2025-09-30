#include "setrankdialog.h"
#include "ui_setrankdialog.h"

setRankDialog::setRankDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setRankDialog)
{
    ui->setupUi(this);
}

setRankDialog::~setRankDialog()
{
    delete ui;
}

char setRankDialog::getRank() {
    if (ui->radioButton_c->isChecked()) { return 'c'; }
    else if (ui->radioButton_h->isChecked()) { return 'h'; }
    else if (ui->radioButton_s->isChecked()) { return 's'; }
    else if (ui->radioButton_u->isChecked()) { return 'u'; }
    else { return '-'; }
}

void setRankDialog::setUsername(QString name) {
    this->ui->label_info->setText(name);
}
