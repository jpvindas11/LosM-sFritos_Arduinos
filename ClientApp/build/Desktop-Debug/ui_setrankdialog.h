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
    QRadioButton *radioButton_u;
    QRadioButton *radioButton_o;

    void setupUi(QDialog *setRankDialog)
    {
        if (setRankDialog->objectName().isEmpty())
            setRankDialog->setObjectName(QString::fromUtf8("setRankDialog"));
        setRankDialog->resize(450, 350);
        setRankDialog->setStyleSheet(QString::fromUtf8("/* ============================================\n"
"   DI\303\201LOGO CAMBIAR RANGO - TEMA NE\303\223N CYBERPUNK\n"
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
"   LABEL INFO\n"
"   ============================================ */\n"
"\n"
"QLabel#label_info {\n"
"    color: #00ffff;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgba(123, 47, 247, 150),\n"
"                                stop:0.5 rgba(0, 212, 255, 150),\n"
"                                stop:1 rgba(255, 0, 110, 150));\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 12px;\n"
" "
                        "   padding: 12px;\n"
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
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
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
"    font-weigh"
                        "t: bold;\n"
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
"    border: 2px solid #00ffff;\n"
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
"/* Radio Button Due\303\261o con estilo especial */\n"
"QRadioButton#radio"
                        "Button_o::indicator {\n"
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
"   DIALOG BUTTON BOX\n"
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
""
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
"QDialogButtonBox QPushButton[text=\"Aceptar\"]:pressed {\n"
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
"    border-radius: 12px"
                        ";\n"
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
"    box-shadow: 0 0 25px rgba(255, 0, 110, 0.8);\n"
"}\n"
"\n"
"QDialogButtonBox QPushButton[text=\"Cancel\"]:pressed,\n"
"QDialogButtonBox QPushButton[text=\"Cancelar\"]:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #cc0058, stop:1 #cc3333);\n"
"    padding-top: 14px;\n"
"    padding-bottom: 10px;\n"
"}"));
        buttonBox = new QDialogButtonBox(setRankDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 280, 330, 50));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_info = new QLabel(setRankDialog);
        label_info->setObjectName(QString::fromUtf8("label_info"));
        label_info->setGeometry(QRect(30, 30, 390, 60));
        label_info->setAlignment(Qt::AlignCenter);
        groupBox_usertype = new QGroupBox(setRankDialog);
        groupBox_usertype->setObjectName(QString::fromUtf8("groupBox_usertype"));
        groupBox_usertype->setGeometry(QRect(110, 120, 230, 140));
        groupBox_usertype->setFlat(false);
        groupBox_usertype->setCheckable(false);
        radioButton_c = new QRadioButton(groupBox_usertype);
        radioButton_c->setObjectName(QString::fromUtf8("radioButton_c"));
        radioButton_c->setGeometry(QRect(15, 30, 200, 24));
        radioButton_h = new QRadioButton(groupBox_usertype);
        radioButton_h->setObjectName(QString::fromUtf8("radioButton_h"));
        radioButton_h->setGeometry(QRect(15, 55, 200, 24));
        radioButton_u = new QRadioButton(groupBox_usertype);
        radioButton_u->setObjectName(QString::fromUtf8("radioButton_u"));
        radioButton_u->setGeometry(QRect(15, 80, 200, 24));
        radioButton_o = new QRadioButton(groupBox_usertype);
        radioButton_o->setObjectName(QString::fromUtf8("radioButton_o"));
        radioButton_o->setGeometry(QRect(15, 105, 200, 24));

        retranslateUi(setRankDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), setRankDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), setRankDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(setRankDialog);
    } // setupUi

    void retranslateUi(QDialog *setRankDialog)
    {
        setRankDialog->setWindowTitle(QCoreApplication::translate("setRankDialog", "\342\255\220 Cambiar Permisos", nullptr));
        label_info->setText(QCoreApplication::translate("setRankDialog", "\342\255\220 Cambiar Permisos de Usuario", nullptr));
        groupBox_usertype->setTitle(QCoreApplication::translate("setRankDialog", "\360\237\221\245 Nuevo Tipo", nullptr));
        radioButton_c->setText(QCoreApplication::translate("setRankDialog", "\360\237\223\212 Consultante", nullptr));
        radioButton_h->setText(QCoreApplication::translate("setRankDialog", "\360\237\222\273 Admin. Equipo", nullptr));
        radioButton_u->setText(QCoreApplication::translate("setRankDialog", "\360\237\221\245 Admin. Usuarios", nullptr));
        radioButton_o->setText(QCoreApplication::translate("setRankDialog", "\360\237\221\221 Due\303\261o", nullptr));
    } // retranslateUi

};

namespace Ui {
    class setRankDialog: public Ui_setRankDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETRANKDIALOG_H
