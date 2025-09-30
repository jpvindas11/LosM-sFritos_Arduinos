/********************************************************************************
** Form generated from reading UI file 'changepassdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHANGEPASSDIALOG_H
#define UI_CHANGEPASSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_changePassDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_password;
    QLineEdit *lineEdit_username;
    QLabel *label_userchangepass;

    void setupUi(QDialog *changePassDialog)
    {
        if (changePassDialog->objectName().isEmpty())
            changePassDialog->setObjectName(QString::fromUtf8("changePassDialog"));
        changePassDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(changePassDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_password = new QLineEdit(changePassDialog);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(30, 160, 341, 26));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        lineEdit_username = new QLineEdit(changePassDialog);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));
        lineEdit_username->setGeometry(QRect(30, 100, 341, 26));
        lineEdit_username->setEchoMode(QLineEdit::Password);
        label_userchangepass = new QLabel(changePassDialog);
        label_userchangepass->setObjectName(QString::fromUtf8("label_userchangepass"));
        label_userchangepass->setGeometry(QRect(35, 40, 331, 20));
        label_userchangepass->setAlignment(Qt::AlignCenter);

        retranslateUi(changePassDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), changePassDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), changePassDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(changePassDialog);
    } // setupUi

    void retranslateUi(QDialog *changePassDialog)
    {
        changePassDialog->setWindowTitle(QCoreApplication::translate("changePassDialog", "Dialog", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("changePassDialog", "Repita nueva contrase\303\261a", nullptr));
        lineEdit_username->setInputMask(QString());
        lineEdit_username->setText(QString());
        lineEdit_username->setPlaceholderText(QCoreApplication::translate("changePassDialog", "Nueva contrase\303\261a", nullptr));
        label_userchangepass->setText(QCoreApplication::translate("changePassDialog", "TEXT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class changePassDialog: public Ui_changePassDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANGEPASSDIALOG_H
