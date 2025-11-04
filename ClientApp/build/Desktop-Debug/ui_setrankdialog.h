/********************************************************************************
** Form generated from reading UI file 'setrankdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETRANKDIALOG_H
#define UI_SETRANKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_setRankDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_info;
    QGroupBox *groupBox_usertype;
    QRadioButton *radioButton_c;
    QRadioButton *radioButton_h;
    QRadioButton *radioButton_o;
    QRadioButton *radioButton_u;

    void setupUi(QDialog *setRankDialog)
    {
        if (setRankDialog->objectName().isEmpty())
            setRankDialog->setObjectName(QString::fromUtf8("setRankDialog"));
        setRankDialog->resize(400, 237);
        buttonBox = new QDialogButtonBox(setRankDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 200, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_info = new QLabel(setRankDialog);
        label_info->setObjectName(QString::fromUtf8("label_info"));
        label_info->setGeometry(QRect(5, 20, 391, 20));
        label_info->setAlignment(Qt::AlignCenter);
        groupBox_usertype = new QGroupBox(setRankDialog);
        groupBox_usertype->setObjectName(QString::fromUtf8("groupBox_usertype"));
        groupBox_usertype->setGeometry(QRect(130, 70, 151, 111));
        groupBox_usertype->setFlat(false);
        groupBox_usertype->setCheckable(false);
        radioButton_c = new QRadioButton(groupBox_usertype);
        radioButton_c->setObjectName(QString::fromUtf8("radioButton_c"));
        radioButton_c->setGeometry(QRect(0, 20, 121, 24));
        radioButton_h = new QRadioButton(groupBox_usertype);
        radioButton_h->setObjectName(QString::fromUtf8("radioButton_h"));
        radioButton_h->setGeometry(QRect(0, 40, 141, 24));
        radioButton_o = new QRadioButton(groupBox_usertype);
        radioButton_o->setObjectName(QString::fromUtf8("radioButton_o"));
        radioButton_o->setGeometry(QRect(0, 80, 141, 24));
        radioButton_u = new QRadioButton(groupBox_usertype);
        radioButton_u->setObjectName(QString::fromUtf8("radioButton_u"));
        radioButton_u->setGeometry(QRect(0, 60, 141, 24));

        retranslateUi(setRankDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), setRankDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), setRankDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(setRankDialog);
    } // setupUi

    void retranslateUi(QDialog *setRankDialog)
    {
        setRankDialog->setWindowTitle(QCoreApplication::translate("setRankDialog", "Dialog", nullptr));
        label_info->setText(QCoreApplication::translate("setRankDialog", "TEXT", nullptr));
        groupBox_usertype->setTitle(QCoreApplication::translate("setRankDialog", "Tipo de Usuario", nullptr));
        radioButton_c->setText(QCoreApplication::translate("setRankDialog", "Consultante", nullptr));
        radioButton_h->setText(QCoreApplication::translate("setRankDialog", "Admin. Hardware", nullptr));
        radioButton_o->setText(QCoreApplication::translate("setRankDialog", "Due\303\261o", nullptr));
        radioButton_u->setText(QCoreApplication::translate("setRankDialog", "Admin. Usuarios", nullptr));
    } // retranslateUi

};

namespace Ui {
    class setRankDialog: public Ui_setRankDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETRANKDIALOG_H
