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
        changePassDialog->resize(450, 350);
        changePassDialog->setStyleSheet(QString::fromUtf8("/* ============================================\n"
"   DI\303\201LOGO - TEMA NE\303\223N CYBERPUNK\n"
"   ============================================ */\n"
"\n"
"QDialog {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                stop:0 #0a0e27, stop:0.5 #16213e, stop:1 #0a0e27);\n"
"    border: 3px solid #7b2ff7;\n"
"    border-radius: 15px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   LABEL T\303\215TULO\n"
"   ============================================ */\n"
"\n"
"QLabel#label_userchangepass {\n"
"    color: #00ffff;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgba(123, 47, 247, 150),\n"
"                                stop:0.5 rgba(0, 212, 255, 150),\n"
"                                stop:1 rgba(255, 0, 110, 150));\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 10px;\n"
""
                        "    padding: 10px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   LINE EDITS (CAMPOS DE TEXTO)\n"
"   ============================================ */\n"
"\n"
"QLineEdit {\n"
"    background: rgba(26, 35, 62, 180);\n"
"    border: 2px solid #7b2ff7;\n"
"    border-radius: 12px;\n"
"    padding: 10px 15px;\n"
"    color: #ffffff;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    selection-background-color: #00d4ff;\n"
"    selection-color: #000000;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 2px solid #00d4ff;\n"
"    background: rgba(26, 35, 62, 220);\n"
"    box-shadow: 0 0 20px rgba(0, 212, 255, 0.6);\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"    border: 2px solid #9351ff;\n"
"    background: rgba(26, 35, 62, 200);\n"
"}\n"
"\n"
"QLineEdit::placeholder {\n"
"    color: rgba(255, 255, 255, 100);\n"
"    font-style: italic;\n"
"}\n"
"\n"
"/* ============================================\n"
"   DIALOG BUTTON BOX\n"
"   ====================================="
                        "======= */\n"
"\n"
"QDialogButtonBox {\n"
"    background: transparent;\n"
"}\n"
"\n"
"/* Bot\303\263n OK/Aceptar */\n"
"QDialogButtonBox QPushButton[text=\"OK\"],\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"] {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00d4ff, stop:1 #7b2ff7);\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 12px;\n"
"    padding: 10px 25px;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 100px;\n"
"    min-height: 35px;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00e5ff, stop:1 #9351ff);\n"
"    box-shadow: 0 0 25px rgba(0, 212, 255, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"]:pressed,\n"
"QDialogButtonBox QPushButton[text="
                        "\"Aceptar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00a8cc, stop:1 #6020dd);\n"
"    padding-top: 12px;\n"
"    padding-bottom: 8px;\n"
"}\n"
"\n"
"/* Bot\303\263n Cancel/Cancelar */\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"],\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"] {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff006e, stop:1 #ff4444);\n"
"    border: 2px solid #ff006e;\n"
"    border-radius: 12px;\n"
"    padding: 10px 25px;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 100px;\n"
"    min-height: 35px;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff1a7a, stop:1 #ff6666);\n"
"  "
                        "  box-shadow: 0 0 25px rgba(255, 0, 110, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"]:pressed,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #cc0058, stop:1 #cc3333);\n"
"    padding-top: 12px;\n"
"    padding-bottom: 8px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   EFECTOS ADICIONALES\n"
"   ============================================ */\n"
"\n"
"QLineEdit#lineEdit_username {\n"
"    border-left: 4px solid #00d4ff;\n"
"}\n"
"\n"
"QLineEdit#lineEdit_password {\n"
"    border-left: 4px solid #ff006e;\n"
"}"));
        buttonBox = new QDialogButtonBox(changePassDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 270, 330, 50));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_password = new QLineEdit(changePassDialog);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(40, 190, 370, 45));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        lineEdit_username = new QLineEdit(changePassDialog);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));
        lineEdit_username->setGeometry(QRect(40, 120, 370, 45));
        lineEdit_username->setEchoMode(QLineEdit::Password);
        label_userchangepass = new QLabel(changePassDialog);
        label_userchangepass->setObjectName(QString::fromUtf8("label_userchangepass"));
        label_userchangepass->setGeometry(QRect(40, 30, 370, 60));
        label_userchangepass->setAlignment(Qt::AlignCenter);

        retranslateUi(changePassDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), changePassDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), changePassDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(changePassDialog);
    } // setupUi

    void retranslateUi(QDialog *changePassDialog)
    {
        changePassDialog->setWindowTitle(QCoreApplication::translate("changePassDialog", "\360\237\224\220 Cambiar Contrase\303\261a", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("changePassDialog", "\360\237\224\222 Repita nueva contrase\303\261a", nullptr));
        lineEdit_username->setInputMask(QString());
        lineEdit_username->setText(QString());
        lineEdit_username->setPlaceholderText(QCoreApplication::translate("changePassDialog", "\360\237\224\221 Nueva contrase\303\261a", nullptr));
        label_userchangepass->setText(QCoreApplication::translate("changePassDialog", "\360\237\224\220 Cambiar Contrase\303\261a de Usuario", nullptr));
    } // retranslateUi

};

namespace Ui {
    class changePassDialog: public Ui_changePassDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANGEPASSDIALOG_H
