/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_main;
    QHBoxLayout *horizontalLayout_top;
    QSpacerItem *horizontalSpacer_left_top;
    QPushButton *pushButton_ip;
    QSpacerItem *verticalSpacer_top;
    QVBoxLayout *verticalLayout_center;
    QLabel *glow_line_top;
    QLabel *label;
    QLabel *subtitle;
    QSpacerItem *verticalSpacer_title;
    QHBoxLayout *horizontalLayout_user;
    QSpacerItem *horizontalSpacer_left_user;
    QLineEdit *lineEdit_user;
    QSpacerItem *horizontalSpacer_right_user;
    QHBoxLayout *horizontalLayout_pass;
    QSpacerItem *horizontalSpacer_left_pass;
    QLineEdit *lineEdit_pass;
    QSpacerItem *horizontalSpacer_right_pass;
    QLabel *login_info;
    QHBoxLayout *horizontalLayout_button;
    QSpacerItem *horizontalSpacer_left_button;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_right_button;
    QLabel *glow_line_bottom;
    QSpacerItem *verticalSpacer_bottom;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 608);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                stop:0 #0a0e27, stop:0.5 #16213e, stop:1 #0a0e27);\n"
"}\n"
"\n"
"QWidget#centralwidget {\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    background-color: rgba(26, 35, 62, 180);\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 15px;\n"
"    padding: 8px 15px;\n"
"    color: #ffffff;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    selection-background-color: #ff006e;\n"
"    min-height: 35px;\n"
"    max-width: 400px;\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"    border: 2px solid #00ffff;\n"
"    background-color: rgba(26, 35, 62, 220);\n"
"    box-shadow: 0 0 15px rgba(0, 212, 255, 0.5);\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 2px solid #ff006e;\n"
"    background-color: rgba(26, 35, 62, 250);\n"
"    box-shadow: 0 0 25px rgba(255, 0, 110, 0.6);\n"
"}\n"
"\n"
"QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0"
                        ",\n"
"                                stop:0 #ff006e, stop:1 #00d4ff);\n"
"    border: none;\n"
"    border-radius: 20px;\n"
"    padding: 12px 30px;\n"
"    color: #ffffff;\n"
"    font-size: 15px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-height: 40px;\n"
"    min-width: 150px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff1a7a, stop:1 #00e5ff);\n"
"    box-shadow: 0 0 20px rgba(255, 0, 110, 0.7);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #cc0058, stop:1 #00a8cc);\n"
"}\n"
"\n"
"QPushButton#pushButton_ip {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    font-size: 13px;\n"
"    padding: 10px 20px;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton#pushButton_ip:hover {\n"
"    back"
                        "ground: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #9351ff, stop:1 #00e5ff);\n"
"    box-shadow: 0 0 20px rgba(123, 47, 247, 0.7);\n"
"}\n"
"\n"
"QLabel#label {\n"
"    color: #00ffff;\n"
"    font-size: 42px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLabel#login_info {\n"
"    color: #ff006e;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLabel#subtitle {\n"
"    color: #00d4ff;\n"
"    font-size: 14px;\n"
"    font-style: italic;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLabel#glow_line {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 transparent, stop:0.3 #00ffff, \n"
"                                stop:0.7 #ff006e, stop:1 transparent);\n"
"    border-radius: 2px;\n"
"    min-height: 3px;\n"
""
                        "    max-height: 3px;\n"
"}\n"
"\n"
"QMenuBar {\n"
"    background-color: #0a0e27;\n"
"    color: #00d4ff;\n"
"    border-bottom: 1px solid #00d4ff;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #16213e;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #0a0e27;\n"
"    color: #00d4ff;\n"
"    border-top: 1px solid #00d4ff;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_main = new QVBoxLayout(centralwidget);
        verticalLayout_main->setSpacing(0);
        verticalLayout_main->setObjectName(QString::fromUtf8("verticalLayout_main"));
        verticalLayout_main->setContentsMargins(20, 20, 20, 20);
        horizontalLayout_top = new QHBoxLayout();
        horizontalLayout_top->setObjectName(QString::fromUtf8("horizontalLayout_top"));
        horizontalSpacer_left_top = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_top->addItem(horizontalSpacer_left_top);

        pushButton_ip = new QPushButton(centralwidget);
        pushButton_ip->setObjectName(QString::fromUtf8("pushButton_ip"));

        horizontalLayout_top->addWidget(pushButton_ip);


        verticalLayout_main->addLayout(horizontalLayout_top);

        verticalSpacer_top = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_main->addItem(verticalSpacer_top);

        verticalLayout_center = new QVBoxLayout();
        verticalLayout_center->setSpacing(20);
        verticalLayout_center->setObjectName(QString::fromUtf8("verticalLayout_center"));
        glow_line_top = new QLabel(centralwidget);
        glow_line_top->setObjectName(QString::fromUtf8("glow_line_top"));
        glow_line_top->setStyleSheet(QString::fromUtf8("background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 transparent, stop:0.3 #00ffff, \n"
"                                stop:0.7 #ff006e, stop:1 transparent);\n"
"border-radius: 2px;\n"
"min-height: 3px;\n"
"max-height: 3px;"));

        verticalLayout_center->addWidget(glow_line_top);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_center->addWidget(label);

        subtitle = new QLabel(centralwidget);
        subtitle->setObjectName(QString::fromUtf8("subtitle"));
        subtitle->setAlignment(Qt::AlignCenter);

        verticalLayout_center->addWidget(subtitle);

        verticalSpacer_title = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_center->addItem(verticalSpacer_title);

        horizontalLayout_user = new QHBoxLayout();
        horizontalLayout_user->setObjectName(QString::fromUtf8("horizontalLayout_user"));
        horizontalSpacer_left_user = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_user->addItem(horizontalSpacer_left_user);

        lineEdit_user = new QLineEdit(centralwidget);
        lineEdit_user->setObjectName(QString::fromUtf8("lineEdit_user"));
        lineEdit_user->setEchoMode(QLineEdit::Normal);
        lineEdit_user->setCursorMoveStyle(Qt::LogicalMoveStyle);

        horizontalLayout_user->addWidget(lineEdit_user);

        horizontalSpacer_right_user = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_user->addItem(horizontalSpacer_right_user);


        verticalLayout_center->addLayout(horizontalLayout_user);

        horizontalLayout_pass = new QHBoxLayout();
        horizontalLayout_pass->setObjectName(QString::fromUtf8("horizontalLayout_pass"));
        horizontalSpacer_left_pass = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_pass->addItem(horizontalSpacer_left_pass);

        lineEdit_pass = new QLineEdit(centralwidget);
        lineEdit_pass->setObjectName(QString::fromUtf8("lineEdit_pass"));
        lineEdit_pass->setEchoMode(QLineEdit::Password);
        lineEdit_pass->setReadOnly(false);

        horizontalLayout_pass->addWidget(lineEdit_pass);

        horizontalSpacer_right_pass = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_pass->addItem(horizontalSpacer_right_pass);


        verticalLayout_center->addLayout(horizontalLayout_pass);

        login_info = new QLabel(centralwidget);
        login_info->setObjectName(QString::fromUtf8("login_info"));
        login_info->setAlignment(Qt::AlignCenter);

        verticalLayout_center->addWidget(login_info);

        horizontalLayout_button = new QHBoxLayout();
        horizontalLayout_button->setObjectName(QString::fromUtf8("horizontalLayout_button"));
        horizontalSpacer_left_button = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_button->addItem(horizontalSpacer_left_button);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_button->addWidget(pushButton);

        horizontalSpacer_right_button = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_button->addItem(horizontalSpacer_right_button);


        verticalLayout_center->addLayout(horizontalLayout_button);

        glow_line_bottom = new QLabel(centralwidget);
        glow_line_bottom->setObjectName(QString::fromUtf8("glow_line_bottom"));
        glow_line_bottom->setStyleSheet(QString::fromUtf8("background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 transparent, stop:0.3 #ff006e, \n"
"                                stop:0.7 #00ffff, stop:1 transparent);\n"
"border-radius: 2px;\n"
"min-height: 3px;\n"
"max-height: 3px;"));

        verticalLayout_center->addWidget(glow_line_bottom);


        verticalLayout_main->addLayout(verticalLayout_center);

        verticalSpacer_bottom = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_main->addItem(verticalSpacer_bottom);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Frituinos - Login", nullptr));
        pushButton_ip->setText(QCoreApplication::translate("MainWindow", "\360\237\214\220 Verificar IP", nullptr));
        glow_line_top->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "FRITUINOS", nullptr));
        subtitle->setText(QCoreApplication::translate("MainWindow", "Tu sede inteligente de confianza", nullptr));
        lineEdit_user->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\221\244 Nombre de Usuario", nullptr));
        lineEdit_pass->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\222 Contrase\303\261a", nullptr));
        login_info->setText(QString());
        pushButton->setText(QCoreApplication::translate("MainWindow", "INGRESAR", nullptr));
        glow_line_bottom->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
