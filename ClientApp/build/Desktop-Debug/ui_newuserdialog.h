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
    QRadioButton *radioButton_u;
    QRadioButton *radioButton_o;

    void setupUi(QDialog *newUserDialog)
    {
        if (newUserDialog->objectName().isEmpty())
            newUserDialog->setObjectName(QString::fromUtf8("newUserDialog"));
        newUserDialog->resize(480, 450);
        newUserDialog->setStyleSheet(QString::fromUtf8("/* ============================================\n"
"   DI\303\201LOGO CREAR USUARIO - TEMA NE\303\223N CYBERPUNK\n"
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
"   LABELS\n"
"   ============================================ */\n"
"\n"
"QLabel#label_3 {\n"
"    color: #00ffff;\n"
"    font-size: 20px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgba(123, 47, 247, 150),\n"
"                                stop:0.5 rgba(0, 212, 255, 150),\n"
"                                stop:1 rgba(123, 47, 247, 150));\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 12px;\n"
"    pad"
                        "ding: 12px;\n"
"}\n"
"\n"
"QLabel#label_error {\n"
"    color: #ff006e;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: rgba(255, 0, 110, 50);\n"
"    border: 2px solid #ff006e;\n"
"    border-radius: 8px;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   LINE EDITS\n"
"   ============================================ */\n"
"\n"
"QLineEdit {\n"
"    background: rgba(26, 35, 62, 180);\n"
"    border: 2px solid #7b2ff7;\n"
"    border-radius: 12px;\n"
"    padding: 12px 15px;\n"
"    color: #ffffff;\n"
"    font-size: 14px;\n"
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
"    backgroun"
                        "d: rgba(26, 35, 62, 200);\n"
"}\n"
"\n"
"QLineEdit::placeholder {\n"
"    color: rgba(255, 255, 255, 100);\n"
"    font-style: italic;\n"
"}\n"
"\n"
"QLineEdit#lineEdit_username {\n"
"    border-left: 4px solid #00d4ff;\n"
"}\n"
"\n"
"QLineEdit#lineEdit_password {\n"
"    border-left: 4px solid #ff006e;\n"
"}\n"
"\n"
"/* ============================================\n"
"   GROUPBOX - TIPO DE USUARIO\n"
"   ============================================ */\n"
"\n"
"QGroupBox {\n"
"    background: rgba(26, 35, 62, 150);\n"
"    border: 2px solid #7b2ff7;\n"
"    border-radius: 12px;\n"
"    color: #00ffff;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding-top: 25px;\n"
"    margin-top: 10px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top center;\n"
"    padding: 5px 15px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00"
                        "d4ff);\n"
"    border-radius: 8px;\n"
"    color: white;\n"
"}\n"
"\n"
"/* ============================================\n"
"   RADIO BUTTONS\n"
"   ============================================ */\n"
"\n"
"QRadioButton {\n"
"    color: #ffffff;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    spacing: 8px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width: 20px;\n"
"    height: 20px;\n"
"    border: 2px solid #7b2ff7;\n"
"    border-radius: 10px;\n"
"    background: rgba(26, 35, 62, 180);\n"
"}\n"
"\n"
"QRadioButton::indicator:hover {\n"
"    border: 2px solid #00d4ff;\n"
"    background: rgba(26, 35, 62, 220);\n"
"    box-shadow: 0 0 10px rgba(0, 212, 255, 0.5);\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,\n"
"                                fx:0.5, fy:0.5,\n"
"                                stop:0 #00ffff, stop:0.5 #00d4ff, stop:1 #7b2ff7);\n"
"    bord"
                        "er: 2px solid #00ffff;\n"
"    box-shadow: 0 0 15px rgba(0, 255, 255, 0.8);\n"
"}\n"
"\n"
"QRadioButton:hover {\n"
"    color: #00ffff;\n"
"}\n"
"\n"
"QRadioButton:checked {\n"
"    color: #00ffff;\n"
"}\n"
"\n"
"/* Emojis para cada tipo */\n"
"QRadioButton#radioButton_c::indicator {\n"
"    /* Consultante */\n"
"}\n"
"\n"
"QRadioButton#radioButton_h::indicator {\n"
"    /* Admin. Nodos */\n"
"}\n"
"\n"
"QRadioButton#radioButton_u::indicator {\n"
"    /* Admin. Usuarios */\n"
"}\n"
"\n"
"QRadioButton#radioButton_o::indicator {\n"
"    /* Due\303\261o */\n"
"    border: 2px solid #ff006e;\n"
"}\n"
"\n"
"QRadioButton#radioButton_o::indicator:checked {\n"
"    background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,\n"
"                                fx:0.5, fy:0.5,\n"
"                                stop:0 #ff006e, stop:0.5 #ff4499, stop:1 #7b2ff7);\n"
"    border: 2px solid #ff006e;\n"
"    box-shadow: 0 0 15px rgba(255, 0, 110, 0.8);\n"
"}\n"
"\n"
"/* ============================================\n"
"   DIALOG"
                        " BUTTON BOX\n"
"   ============================================ */\n"
"\n"
"QDialogButtonBox {\n"
"    background: transparent;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"],\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"] {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00d4ff, stop:1 #7b2ff7);\n"
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
"QDialogButtonBox QPushButton[text=\"OK\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Aceptar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00e5ff, stop:1 #9351ff);\n"
"    box-shadow: 0 0 25px rgba(0, 212, 255, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"OK\"]:pressed,\n"
"QDialogButto"
                        "nBox QPushButton[text=\"Aceptar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #00a8cc, stop:1 #6020dd);\n"
"    padding-top: 14px;\n"
"    padding-bottom: 10px;\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"],\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"] {\n"
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
"QDialogButtonBox QPushButton[text=\"Cancel\"]:hover,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff1a7a, stop:1 #ff6666);\n"
"    box-shadow: 0 "
                        "0 25px rgba(255, 0, 110, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"]:pressed,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #cc0058, stop:1 #cc3333);\n"
"    padding-top: 14px;\n"
"    padding-bottom: 10px;\n"
"}"));
        buttonBox = new QDialogButtonBox(newUserDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(75, 380, 330, 50));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit_username = new QLineEdit(newUserDialog);
        lineEdit_username->setObjectName(QString::fromUtf8("lineEdit_username"));
        lineEdit_username->setGeometry(QRect(40, 90, 400, 45));
        lineEdit_password = new QLineEdit(newUserDialog);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(40, 155, 400, 45));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        label_error = new QLabel(newUserDialog);
        label_error->setObjectName(QString::fromUtf8("label_error"));
        label_error->setGeometry(QRect(40, 340, 400, 30));
        label_error->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(newUserDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 20, 400, 50));
        label_3->setAlignment(Qt::AlignCenter);
        groupBox_usertype = new QGroupBox(newUserDialog);
        groupBox_usertype->setObjectName(QString::fromUtf8("groupBox_usertype"));
        groupBox_usertype->setGeometry(QRect(125, 220, 230, 110));
        groupBox_usertype->setFlat(false);
        groupBox_usertype->setCheckable(false);
        radioButton_c = new QRadioButton(groupBox_usertype);
        radioButton_c->setObjectName(QString::fromUtf8("radioButton_c"));
        radioButton_c->setGeometry(QRect(15, 30, 200, 24));
        radioButton_h = new QRadioButton(groupBox_usertype);
        radioButton_h->setObjectName(QString::fromUtf8("radioButton_h"));
        radioButton_h->setGeometry(QRect(15, 50, 200, 24));
        radioButton_u = new QRadioButton(groupBox_usertype);
        radioButton_u->setObjectName(QString::fromUtf8("radioButton_u"));
        radioButton_u->setGeometry(QRect(15, 70, 200, 24));
        radioButton_o = new QRadioButton(groupBox_usertype);
        radioButton_o->setObjectName(QString::fromUtf8("radioButton_o"));
        radioButton_o->setGeometry(QRect(15, 90, 200, 24));

        retranslateUi(newUserDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), newUserDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), newUserDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(newUserDialog);
    } // setupUi

    void retranslateUi(QDialog *newUserDialog)
    {
        newUserDialog->setWindowTitle(QCoreApplication::translate("newUserDialog", "\342\236\225 Crear Usuario", nullptr));
        lineEdit_username->setText(QString());
        lineEdit_username->setPlaceholderText(QCoreApplication::translate("newUserDialog", "\360\237\221\244 Nombre de usuario", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("newUserDialog", "\360\237\224\222 Contrase\303\261a", nullptr));
        label_error->setText(QString());
        label_3->setText(QCoreApplication::translate("newUserDialog", "\342\236\225 Crear Usuario Nuevo", nullptr));
        groupBox_usertype->setTitle(QCoreApplication::translate("newUserDialog", "\360\237\221\245 Tipo de Usuario", nullptr));
        radioButton_c->setText(QCoreApplication::translate("newUserDialog", "\360\237\223\212 Consultante", nullptr));
        radioButton_h->setText(QCoreApplication::translate("newUserDialog", "\360\237\214\220 Admin. Nodos", nullptr));
        radioButton_u->setText(QCoreApplication::translate("newUserDialog", "\360\237\221\245 Admin. Usuarios", nullptr));
        radioButton_o->setText(QCoreApplication::translate("newUserDialog", "\360\237\221\221 Due\303\261o", nullptr));
    } // retranslateUi

};

namespace Ui {
    class newUserDialog: public Ui_newUserDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWUSERDIALOG_H
