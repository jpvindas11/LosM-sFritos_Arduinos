/********************************************************************************
** Form generated from reading UI file 'confirmdeleteuserdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIRMDELETEUSERDIALOG_H
#define UI_CONFIRMDELETEUSERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_confirmDeleteUserDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_warning;

    void setupUi(QDialog *confirmDeleteUserDialog)
    {
        if (confirmDeleteUserDialog->objectName().isEmpty())
            confirmDeleteUserDialog->setObjectName(QString::fromUtf8("confirmDeleteUserDialog"));
        confirmDeleteUserDialog->resize(380, 105);
        buttonBox = new QDialogButtonBox(confirmDeleteUserDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(-70, 60, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_warning = new QLabel(confirmDeleteUserDialog);
        label_warning->setObjectName(QString::fromUtf8("label_warning"));
        label_warning->setGeometry(QRect(0, 10, 371, 41));
        label_warning->setAlignment(Qt::AlignCenter);

        retranslateUi(confirmDeleteUserDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), confirmDeleteUserDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), confirmDeleteUserDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(confirmDeleteUserDialog);
    } // setupUi

    void retranslateUi(QDialog *confirmDeleteUserDialog)
    {
        confirmDeleteUserDialog->setWindowTitle(QCoreApplication::translate("confirmDeleteUserDialog", "Dialog", nullptr));
        label_warning->setText(QCoreApplication::translate("confirmDeleteUserDialog", "TEXT DELETE USER", nullptr));
    } // retranslateUi

};

namespace Ui {
    class confirmDeleteUserDialog: public Ui_confirmDeleteUserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIRMDELETEUSERDIALOG_H
