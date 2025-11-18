/********************************************************************************
** Form generated from reading UI file 'menuwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENUWINDOW_H
#define UI_MENUWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MenuWindow
{
public:
    QWidget *centralwidget;
    QLabel *main_label;
    QGroupBox *groupBox;
    QPushButton *b_usuarios;
    QPushButton *b_nodos;
    QPushButton *b_consulta;
    QPushButton *b_compus;
    QPushButton *b_cerrarSesion;
    QPushButton *user_add;
    QPushButton *user_delete;
    QListWidget *user_list;
    QPushButton *user_change_pass;
    QPushButton *user_change_rank;
    QLabel *label_cambios;
    QListWidget *arduino_list;
    QPushButton *arduino_turn;
    QListWidget *data_list;
    QPushButton *b_consultar;
    QPushButton *pc1_button;
    QPushButton *pc2_button;
    QPushButton *pc3_button;
    QPushButton *pc4_button;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MenuWindow)
    {
        if (MenuWindow->objectName().isEmpty())
            MenuWindow->setObjectName(QString::fromUtf8("MenuWindow"));
        MenuWindow->resize(800, 580);
        centralwidget = new QWidget(MenuWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        main_label = new QLabel(centralwidget);
        main_label->setObjectName(QString::fromUtf8("main_label"));
        main_label->setGeometry(QRect(140, 0, 661, 20));
        main_label->setAlignment(Qt::AlignCenter);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 141, 551));
        b_usuarios = new QPushButton(groupBox);
        b_usuarios->setObjectName(QString::fromUtf8("b_usuarios"));
        b_usuarios->setGeometry(QRect(0, 60, 141, 26));
        b_nodos = new QPushButton(groupBox);
        b_nodos->setObjectName(QString::fromUtf8("b_nodos"));
        b_nodos->setGeometry(QRect(0, 90, 141, 26));
        b_consulta = new QPushButton(groupBox);
        b_consulta->setObjectName(QString::fromUtf8("b_consulta"));
        b_consulta->setGeometry(QRect(0, 150, 141, 26));
        b_compus = new QPushButton(groupBox);
        b_compus->setObjectName(QString::fromUtf8("b_compus"));
        b_compus->setGeometry(QRect(0, 180, 141, 26));
        b_cerrarSesion = new QPushButton(centralwidget);
        b_cerrarSesion->setObjectName(QString::fromUtf8("b_cerrarSesion"));
        b_cerrarSesion->setGeometry(QRect(0, 520, 141, 26));
        user_add = new QPushButton(centralwidget);
        user_add->setObjectName(QString::fromUtf8("user_add"));
        user_add->setGeometry(QRect(170, 30, 87, 26));
        user_delete = new QPushButton(centralwidget);
        user_delete->setObjectName(QString::fromUtf8("user_delete"));
        user_delete->setGeometry(QRect(170, 70, 87, 26));
        user_list = new QListWidget(centralwidget);
        user_list->setObjectName(QString::fromUtf8("user_list"));
        user_list->setGeometry(QRect(510, 70, 501, 481));
        user_change_pass = new QPushButton(centralwidget);
        user_change_pass->setObjectName(QString::fromUtf8("user_change_pass"));
        user_change_pass->setGeometry(QRect(170, 140, 87, 26));
        user_change_rank = new QPushButton(centralwidget);
        user_change_rank->setObjectName(QString::fromUtf8("user_change_rank"));
        user_change_rank->setGeometry(QRect(170, 180, 87, 26));
        label_cambios = new QLabel(centralwidget);
        label_cambios->setObjectName(QString::fromUtf8("label_cambios"));
        label_cambios->setGeometry(QRect(170, 110, 81, 18));
        label_cambios->setAlignment(Qt::AlignCenter);
        arduino_list = new QListWidget(centralwidget);
        arduino_list->setObjectName(QString::fromUtf8("arduino_list"));
        arduino_list->setGeometry(QRect(510, 70, 501, 481));
        arduino_turn = new QPushButton(centralwidget);
        arduino_turn->setObjectName(QString::fromUtf8("arduino_turn"));
        arduino_turn->setGeometry(QRect(170, 230, 87, 31));
        data_list = new QListWidget(centralwidget);
        data_list->setObjectName(QString::fromUtf8("data_list"));
        data_list->setGeometry(QRect(510, 70, 601, 481));
        b_consultar = new QPushButton(centralwidget);
        b_consultar->setObjectName(QString::fromUtf8("b_consultar"));
        b_consultar->setGeometry(QRect(170, 270, 87, 31));
        pc1_button = new QPushButton(centralwidget);
        pc1_button->setObjectName(QString::fromUtf8("pc1_button"));
        pc1_button->setGeometry(QRect(380, 60, 101, 101));
        pc2_button = new QPushButton(centralwidget);
        pc2_button->setObjectName(QString::fromUtf8("pc2_button"));
        pc2_button->setGeometry(QRect(530, 60, 101, 101));
        pc3_button = new QPushButton(centralwidget);
        pc3_button->setObjectName(QString::fromUtf8("pc3_button"));
        pc3_button->setGeometry(QRect(380, 210, 101, 101));
        pc4_button = new QPushButton(centralwidget);
        pc4_button->setObjectName(QString::fromUtf8("pc4_button"));
        pc4_button->setGeometry(QRect(530, 210, 101, 101));
        MenuWindow->setCentralWidget(centralwidget);
        b_consultar->raise();
        main_label->raise();
        groupBox->raise();
        b_cerrarSesion->raise();
        user_add->raise();
        user_delete->raise();
        user_list->raise();
        user_change_pass->raise();
        user_change_rank->raise();
        label_cambios->raise();
        arduino_list->raise();
        arduino_turn->raise();
        data_list->raise();
        pc1_button->raise();
        pc2_button->raise();
        pc3_button->raise();
        pc4_button->raise();
        menubar = new QMenuBar(MenuWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MenuWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MenuWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MenuWindow->setStatusBar(statusbar);

        retranslateUi(MenuWindow);

        QMetaObject::connectSlotsByName(MenuWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MenuWindow)
    {
        MenuWindow->setWindowTitle(QCoreApplication::translate("MenuWindow", "MainWindow", nullptr));
        main_label->setText(QCoreApplication::translate("MenuWindow", "Men\303\272 Principal", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MenuWindow", "Opciones", nullptr));
        b_usuarios->setText(QCoreApplication::translate("MenuWindow", "Usuarios", nullptr));
        b_nodos->setText(QCoreApplication::translate("MenuWindow", "Nodos", nullptr));
        b_consulta->setText(QCoreApplication::translate("MenuWindow", "Consulta", nullptr));
        b_compus->setText(QCoreApplication::translate("MenuWindow", "Computadoras", nullptr));
        b_cerrarSesion->setText(QCoreApplication::translate("MenuWindow", "Cerrar Sesi\303\263n", nullptr));
        user_add->setText(QCoreApplication::translate("MenuWindow", "Agregar", nullptr));
        user_delete->setText(QCoreApplication::translate("MenuWindow", "Eliminar", nullptr));
        user_change_pass->setText(QCoreApplication::translate("MenuWindow", "Contrase\303\261a", nullptr));
        user_change_rank->setText(QCoreApplication::translate("MenuWindow", "Permisos", nullptr));
        label_cambios->setText(QCoreApplication::translate("MenuWindow", "Cambios", nullptr));
        arduino_turn->setText(QCoreApplication::translate("MenuWindow", "Encender", nullptr));
        b_consultar->setText(QCoreApplication::translate("MenuWindow", "Consultar", nullptr));
        pc1_button->setText(QCoreApplication::translate("MenuWindow", "PC 1", nullptr));
        pc2_button->setText(QCoreApplication::translate("MenuWindow", "PC 2", nullptr));
        pc3_button->setText(QCoreApplication::translate("MenuWindow", "PC 3", nullptr));
        pc4_button->setText(QCoreApplication::translate("MenuWindow", "PC 4", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MenuWindow: public Ui_MenuWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWINDOW_H
