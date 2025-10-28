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
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *lineEdit_pass;
    QLineEdit *lineEdit_user;
    QLabel *label;
    QPushButton *pushButton;
    QLabel *login_info;
    QLabel *label_2;
    QLineEdit *lineEdit_IP;
    QLineEdit *lineEdit_PORT;
    QPushButton *pushButton_ip;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lineEdit_pass = new QLineEdit(centralwidget);
        lineEdit_pass->setObjectName(QString::fromUtf8("lineEdit_pass"));
        lineEdit_pass->setGeometry(QRect(110, 220, 281, 26));
        lineEdit_pass->setEchoMode(QLineEdit::Password);
        lineEdit_pass->setReadOnly(false);
        lineEdit_user = new QLineEdit(centralwidget);
        lineEdit_user->setObjectName(QString::fromUtf8("lineEdit_user"));
        lineEdit_user->setGeometry(QRect(110, 160, 281, 26));
        lineEdit_user->setEchoMode(QLineEdit::Normal);
        lineEdit_user->setCursorMoveStyle(Qt::LogicalMoveStyle);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(80, 90, 331, 20));
        label->setAlignment(Qt::AlignCenter);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(210, 290, 87, 26));
        login_info = new QLabel(centralwidget);
        login_info->setObjectName(QString::fromUtf8("login_info"));
        login_info->setGeometry(QRect(80, 260, 331, 20));
        login_info->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(450, 90, 331, 20));
        label_2->setAlignment(Qt::AlignCenter);
        lineEdit_IP = new QLineEdit(centralwidget);
        lineEdit_IP->setObjectName(QString::fromUtf8("lineEdit_IP"));
        lineEdit_IP->setGeometry(QRect(480, 160, 281, 26));
        lineEdit_IP->setEchoMode(QLineEdit::Normal);
        lineEdit_IP->setCursorMoveStyle(Qt::LogicalMoveStyle);
        lineEdit_PORT = new QLineEdit(centralwidget);
        lineEdit_PORT->setObjectName(QString::fromUtf8("lineEdit_PORT"));
        lineEdit_PORT->setGeometry(QRect(480, 220, 281, 26));
        lineEdit_PORT->setEchoMode(QLineEdit::Normal);
        lineEdit_PORT->setCursorMoveStyle(Qt::LogicalMoveStyle);
        pushButton_ip = new QPushButton(centralwidget);
        pushButton_ip->setObjectName(QString::fromUtf8("pushButton_ip"));
        pushButton_ip->setGeometry(QRect(580, 290, 87, 26));
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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        lineEdit_pass->setPlaceholderText(QCoreApplication::translate("MainWindow", "Contrase\303\261a", nullptr));
        lineEdit_user->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nombre de Usuario", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Bienvenido a Frituinos", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Ingresar", nullptr));
        login_info->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "Ingresa IP y Puerto", nullptr));
        lineEdit_IP->setPlaceholderText(QCoreApplication::translate("MainWindow", "IP del servidor", nullptr));
        lineEdit_PORT->setText(QString());
        lineEdit_PORT->setPlaceholderText(QCoreApplication::translate("MainWindow", "Puerto del servidor", nullptr));
        pushButton_ip->setText(QCoreApplication::translate("MainWindow", "Verificar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
