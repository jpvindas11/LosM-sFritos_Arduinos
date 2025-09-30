/********************************************************************************
** Form generated from reading UI file 'newuserdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWUSERDIALOG_H
#define UI_NEWUSERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_newUserDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_username;
    QLineEdit *lineEdit_password;
    QLabel *label_error;
    QLabel *label_3;
    QGroupBox *groupBox_usertype;
    QRadioButton *radioButton_c;
    QRadioButton *radioButton_h;
    QRadioButton *radioButton_s;
    QRadioButton *radioButton_u;

    void setupUi(QDialog *newUserDialog)
    {
        if (newUserDialog->objectName().isEmpty())
            newUserDialog->setObjectName(QString::fromUtf8("newUserDialog"));
        newUserDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(newUserDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_username = new QLineEdit(newUserDialog);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));
        lineEdit_username->setGeometry(QRect(30, 70, 341, 26));
        lineEdit_password = new QLineEdit(newUserDialog);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(30, 120, 341, 26));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        label_error = new QLabel(newUserDialog);
        label_error->setObjectName(QString::fromUtf8("label_error"));
        label_error->setGeometry(QRect(35, 200, 331, 20));
        label_error->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(newUserDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(30, 20, 341, 20));
        label_3->setAlignment(Qt::AlignCenter);
        groupBox_usertype = new QGroupBox(newUserDialog);
        groupBox_usertype->setObjectName(QString::fromUtf8("groupBox_usertype"));
        groupBox_usertype->setGeometry(QRect(30, 170, 151, 111));
        groupBox_usertype->setFlat(false);
        groupBox_usertype->setCheckable(false);
        radioButton_c = new QRadioButton(groupBox_usertype);
        radioButton_c->setObjectName(QString::fromUtf8("radioButton_c"));
        radioButton_c->setGeometry(QRect(0, 20, 121, 24));
        radioButton_h = new QRadioButton(groupBox_usertype);
        radioButton_h->setObjectName(QString::fromUtf8("radioButton_h"));
        radioButton_h->setGeometry(QRect(0, 40, 141, 24));
        radioButton_s = new QRadioButton(groupBox_usertype);
        radioButton_s->setObjectName(QString::fromUtf8("radioButton_s"));
        radioButton_s->setGeometry(QRect(0, 60, 141, 24));
        radioButton_u = new QRadioButton(groupBox_usertype);
        radioButton_u->setObjectName(QString::fromUtf8("radioButton_u"));
        radioButton_u->setGeometry(QRect(0, 80, 141, 24));

        retranslateUi(newUserDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), newUserDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), newUserDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(newUserDialog);
    } // setupUi

    void retranslateUi(QDialog *newUserDialog)
    {
        newUserDialog->setWindowTitle(QCoreApplication::translate("newUserDialog", "Dialog", nullptr));
        lineEdit_username->setText(QString());
        lineEdit_username->setPlaceholderText(QCoreApplication::translate("newUserDialog", "Usuario", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("newUserDialog", "Contrase\303\261a", nullptr));
        label_error->setText(QString());
        label_3->setText(QCoreApplication::translate("newUserDialog", "Crear un usuario nuevo", nullptr));
        groupBox_usertype->setTitle(QCoreApplication::translate("newUserDialog", "Tipo de Usuario", nullptr));
        radioButton_c->setText(QCoreApplication::translate("newUserDialog", "Consultante", nullptr));
        radioButton_h->setText(QCoreApplication::translate("newUserDialog", "Admin. Hardware", nullptr));
        radioButton_s->setText(QCoreApplication::translate("newUserDialog", "Admin. Software", nullptr));
        radioButton_u->setText(QCoreApplication::translate("newUserDialog", "Admin. Usuarios", nullptr));
    } // retranslateUi

};

namespace Ui {
    class newUserDialog: public Ui_newUserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWUSERDIALOG_H
