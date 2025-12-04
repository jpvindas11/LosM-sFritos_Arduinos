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
        confirmDeleteUserDialog->resize(450, 180);
        confirmDeleteUserDialog->setStyleSheet(QString::fromUtf8("/* ============================================\n"
"   DI\303\201LOGO DE CONFIRMACI\303\223N - TEMA PELIGRO NE\303\223N\n"
"   ============================================ */\n"
"\n"
"QDialog {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                stop:0 #0a0e27, stop:0.5 #16213e, stop:1 #0a0e27);\n"
"    border: 3px solid #ff006e;\n"
"    border-radius: 15px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   LABEL DE ADVERTENCIA\n"
"   ============================================ */\n"
"\n"
"QLabel#label_warning {\n"
"    color: #ffffff;\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgba(255, 0, 110, 180),\n"
"                                stop:0.5 rgba(255, 68, 68, 180),\n"
"                                stop:1 rgba(255, 0, 110, 180));\n"
"    border: 2px solid #ff006e;\n"
"    border-"
                        "radius: 12px;\n"
"    padding: 15px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   DIALOG BUTTON BOX\n"
"   ============================================ */\n"
"\n"
"QDialogButtonBox {\n"
"    background: transparent;\n"
"}\n"
"\n"
"/* Bot\303\263n OK/Confirmar - ROJO PELIGRO */\n"
"QDialogButtonBox QPushButton[text=\"OK\"],\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"] {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff006e, stop:1 #ff4444);\n"
"    border: 2px solid #ff006e;\n"
"    border-radius: 12px;\n"
"    padding: 12px 30px;\n"
"    color: white;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 120px;\n"
"    min-height: 40px;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop"
                        ":0 #ff1a7a, stop:1 #ff6666);\n"
"    box-shadow: 0 0 30px rgba(255, 0, 110, 1.0);\n"
"    border: 2px solid #ff4499;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"]:pressed,\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #cc0058, stop:1 #cc3333);\n"
"    padding-top: 14px;\n"
"    padding-bottom: 10px;\n"
"}\n"
"\n"
"/* Bot\303\263n Cancel/Cancelar - SEGURO */\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"],\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"] {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 12px;\n"
"    padding: 12px 30px;\n"
"    color: white;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 120px;\n"
"    min-height: 40px;\n"
"}\n"
"\n"
"QDialogButtonBox "
                        "QPushButton[text=\"Cancel\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #9351ff, stop:1 #00e5ff);\n"
"    box-shadow: 0 0 25px rgba(0, 212, 255, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"]:pressed,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #6020dd, stop:1 #00a8cc);\n"
"    padding-top: 14px;\n"
"    padding-bottom: 10px;\n"
"}"));
        buttonBox = new QDialogButtonBox(confirmDeleteUserDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 120, 330, 50));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_warning = new QLabel(confirmDeleteUserDialog);
        label_warning->setObjectName(QString::fromUtf8("label_warning"));
        label_warning->setGeometry(QRect(30, 30, 390, 70));
        label_warning->setAlignment(Qt::AlignCenter);

        retranslateUi(confirmDeleteUserDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), confirmDeleteUserDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), confirmDeleteUserDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(confirmDeleteUserDialog);
    } // setupUi

    void retranslateUi(QDialog *confirmDeleteUserDialog)
    {
        confirmDeleteUserDialog->setWindowTitle(QCoreApplication::translate("confirmDeleteUserDialog", "\342\232\240\357\270\217 Confirmar Eliminaci\303\263n", nullptr));
        label_warning->setText(QCoreApplication::translate("confirmDeleteUserDialog", "\342\232\240\357\270\217 \302\277Est\303\241s seguro de eliminar este usuario?", nullptr));
    } // retranslateUi

};

namespace Ui {
    class confirmDeleteUserDialog: public Ui_confirmDeleteUserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIRMDELETEUSERDIALOG_H
