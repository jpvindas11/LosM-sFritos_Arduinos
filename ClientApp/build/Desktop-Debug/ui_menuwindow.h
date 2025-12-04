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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MenuWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *mainHorizontalLayout;
    QVBoxLayout *sidebarLayout;
    QGroupBox *groupBox;
    QVBoxLayout *menuButtonsLayout;
    QPushButton *b_usuarios;
    QPushButton *b_nodos;
    QPushButton *b_consulta;
    QPushButton *b_compus;
    QSpacerItem *menuSpacer;
    QPushButton *b_cerrarSesion;
    QVBoxLayout *contentLayout;
    QLabel *main_label;
    QStackedWidget *stackedWidget;
    QWidget *page_usuarios;
    QHBoxLayout *usuariosLayout;
    QVBoxLayout *usuariosActionsLayout;
    QPushButton *user_add;
    QPushButton *user_delete;
    QLabel *label_cambios;
    QPushButton *user_change_pass;
    QPushButton *user_change_rank;
    QSpacerItem *verticalSpacer_usuarios;
    QListWidget *user_list;
    QWidget *page_nodos;
    QVBoxLayout *nodosLayout;
    QListWidget *arduino_list;
    QHBoxLayout *nodosButtonsLayout;
    QSpacerItem *horizontalSpacer_nodos;
    QSpacerItem *horizontalSpacer_nodos2;
    QWidget *page_consulta;
    QVBoxLayout *consultaLayout;
    QListWidget *data_list;
    QWidget *page_compus;
    QVBoxLayout *compusMainLayout;
    QSpacerItem *verticalSpacer_compus_top;
    QGridLayout *compusGridLayout;
    QHBoxLayout *pc1_layout;
    QSpacerItem *horizontalSpacer_pc1;
    QPushButton *pc1_button;
    QSpacerItem *horizontalSpacer_pc1_2;
    QHBoxLayout *pc2_layout;
    QSpacerItem *horizontalSpacer_pc2;
    QPushButton *pc2_button;
    QSpacerItem *horizontalSpacer_pc2_2;
    QHBoxLayout *pc3_layout;
    QSpacerItem *horizontalSpacer_pc3;
    QPushButton *pc3_button;
    QSpacerItem *horizontalSpacer_pc3_2;
    QHBoxLayout *pc4_layout;
    QSpacerItem *horizontalSpacer_pc4;
    QPushButton *pc4_button;
    QSpacerItem *horizontalSpacer_pc4_2;
    QSpacerItem *verticalSpacer_compus_bottom;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MenuWindow)
    {
        if (MenuWindow->objectName().isEmpty())
            MenuWindow->setObjectName(QString::fromUtf8("MenuWindow"));
        MenuWindow->resize(1000, 650);
        MenuWindow->setStyleSheet(QString::fromUtf8("/* ============================================\n"
"   ESTILOS GLOBALES - TEMA NE\303\223N CYBERPUNK\n"
"   ============================================ */\n"
"\n"
"QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                                stop:0 #0a0e27, stop:0.5 #16213e, stop:1 #0a0e27);\n"
"}\n"
"\n"
"QWidget#centralwidget {\n"
"    background: transparent;\n"
"}\n"
"\n"
"/* ============================================\n"
"   SIDEBAR - MEN\303\232 LATERAL\n"
"   ============================================ */\n"
"\n"
"QGroupBox {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgba(20, 30, 60, 220), \n"
"                                stop:1 rgba(26, 35, 62, 200));\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 10px;\n"
"    color: #00ffff;\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding-top: 20px;\n"
"    margin-top: 10px;\n"
"}\n"
"\n"
""
                        "QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top center;\n"
"    padding: 5px 15px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff006e, stop:1 #00d4ff);\n"
"    border-radius: 8px;\n"
"    color: white;\n"
"}\n"
"\n"
"/* ============================================\n"
"   BOTONES DEL SIDEBAR\n"
"   ============================================ */\n"
"\n"
"QGroupBox QPushButton {\n"
"    background: rgba(26, 35, 62, 150);\n"
"    border: 2px solid #7b2ff7;\n"
"    border-radius: 12px;\n"
"    padding: 10px;\n"
"    color: #ffffff;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    text-align: left;\n"
"    padding-left: 15px;\n"
"}\n"
"\n"
"QGroupBox QPushButton:hover {\n"
"    background: rgba(123, 47, 247, 100);\n"
"    border: 2px solid #9351ff;\n"
"    box-shadow: 0 0 15px rgba(123, 47, 247, 0.6);\n"
"}\n"
"\n"
"QGroupBox QPushButton:pressed {\n"
"    "
                        "background: rgba(123, 47, 247, 150);\n"
"    padding-top: 12px;\n"
"    padding-bottom: 8px;\n"
"}\n"
"\n"
"QGroupBox QPushButton:disabled {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff006e, stop:1 #00d4ff);\n"
"    border: 2px solid #00ffff;\n"
"    box-shadow: 0 0 20px rgba(0, 255, 255, 0.8);\n"
"    color: white;\n"
"}\n"
"\n"
"/* ============================================\n"
"   BOT\303\223N CERRAR SESI\303\223N\n"
"   ============================================ */\n"
"\n"
"QPushButton#b_cerrarSesion {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #ff006e, stop:1 #ff4444);\n"
"    border: 2px solid #ff006e;\n"
"    border-radius: 15px;\n"
"    padding: 12px;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"QPushButton#b_cerrarSesion:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2"
                        ":0,\n"
"                                stop:0 #ff1a7a, stop:1 #ff6666);\n"
"    box-shadow: 0 0 20px rgba(255, 0, 110, 0.8);\n"
"}\n"
"\n"
"/* ============================================\n"
"   T\303\215TULO PRINCIPAL\n"
"   ============================================ */\n"
"\n"
"QLabel#main_label {\n"
"    color: #00ffff;\n"
"    font-size: 28px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: transparent;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"/* ============================================\n"
"   BOTONES DE ACCI\303\223N (Agregar, Eliminar, etc)\n"
"   ============================================ */\n"
"\n"
"QPushButton#user_add,\n"
"QPushButton#user_delete,\n"
"QPushButton#user_change_pass,\n"
"QPushButton#user_change_rank,\n"
"QPushButton#arduino_turn {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    border: none;\n"
"    border-radius: 15px;\n"
"    padding: 10px 2"
                        "0px;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-height: 35px;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton#user_add:hover,\n"
"QPushButton#user_delete:hover,\n"
"QPushButton#user_change_pass:hover,\n"
"QPushButton#user_change_rank:hover,\n"
"QPushButton#arduino_turn:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #9351ff, stop:1 #00e5ff);\n"
"    box-shadow: 0 0 20px rgba(123, 47, 247, 0.7);\n"
"}\n"
"\n"
"/* ============================================\n"
"   BOTONES DE COMPUTADORAS (PC 1-4)\n"
"   ============================================ */\n"
"\n"
"QPushButton#pc1_button,\n"
"QPushButton#pc2_button,\n"
"QPushButton#pc3_button,\n"
"QPushButton#pc4_button {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 rgba(26, 35, 62, 200),\n"
"                                stop:1 rgba(15, 20, 40, 20"
                        "0));\n"
"    border: 3px solid #00d4ff;\n"
"    border-radius: 20px;\n"
"    color: white;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 140px;\n"
"    min-height: 140px;\n"
"}\n"
"\n"
"QPushButton#pc1_button:hover,\n"
"QPushButton#pc2_button:hover,\n"
"QPushButton#pc3_button:hover,\n"
"QPushButton#pc4_button:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 rgba(123, 47, 247, 150),\n"
"                                stop:1 rgba(0, 212, 255, 150));\n"
"    border: 3px solid #00ffff;\n"
"    box-shadow: 0 0 30px rgba(0, 212, 255, 0.8);\n"
"}\n"
"\n"
"QPushButton#pc1_button:pressed,\n"
"QPushButton#pc2_button:pressed,\n"
"QPushButton#pc3_button:pressed,\n"
"QPushButton#pc4_button:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 rgba(123, 47, 247, 200),\n"
"                                stop:1 rgba(0, 212, 255, "
                        "200));\n"
"}\n"
"\n"
"/* ============================================\n"
"   LISTAS (QListWidget)\n"
"   ============================================ */\n"
"\n"
"QListWidget {\n"
"    background-color: rgba(26, 35, 62, 180);\n"
"    border: 2px solid #00d4ff;\n"
"    border-radius: 15px;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 8px;\n"
"    border-radius: 8px;\n"
"    margin: 2px;\n"
"}\n"
"\n"
"QListWidget::item:hover {\n"
"    background: rgba(123, 47, 247, 100);\n"
"    border: 1px solid #9351ff;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    border: 1px solid #00ffff;\n"
"    color: white;\n"
"}\n"
"\n"
"/* ============================================\n"
"   ETIQUETAS\n"
"   ============================================ */\n"
"\n"
"QLabel#label_"
                        "cambios {\n"
"    color: #ff006e;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background: transparent;\n"
"}\n"
"\n"
"/* ============================================\n"
"   MENU BAR Y STATUS BAR\n"
"   ============================================ */\n"
"\n"
"QMenuBar {\n"
"    background-color: #0a0e27;\n"
"    color: #00d4ff;\n"
"    border-bottom: 2px solid #00d4ff;\n"
"    padding: 3px;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #16213e;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #0a0e27;\n"
"    color: #00d4ff;\n"
"    border-top: 2px solid #00d4ff;\n"
"}\n"
"\n"
"/* ============================================\n"
"   SCROLLBARS\n"
"   ============================================ */\n"
"\n"
"QScrollBar:vertical {\n"
"    background: rgba(26, 35, 62, 100);\n"
"    width: 12px;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: qlineargra"
                        "dient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    border-radius: 6px;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #9351ff, stop:1 #00e5ff);\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    background: rgba(26, 35, 62, 100);\n"
"    height: 12px;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #7b2ff7, stop:1 #00d4ff);\n"
"    border-radius: 6px;\n"
"    min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::add-line, QScrollBar::sub-line {\n"
"    background: none;\n"
"    border: none;\n"
"}\n"
"\n"
"QScrollBar::add-page, QScrollBar::sub-page {\n"
"    background: none;\n"
"}"));
        centralwidget = new QWidget(MenuWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        mainHorizontalLayout = new QHBoxLayout(centralwidget);
        mainHorizontalLayout->setSpacing(15);
        mainHorizontalLayout->setObjectName(QString::fromUtf8("mainHorizontalLayout"));
        mainHorizontalLayout->setContentsMargins(15, 15, 15, 15);
        sidebarLayout = new QVBoxLayout();
        sidebarLayout->setSpacing(10);
        sidebarLayout->setObjectName(QString::fromUtf8("sidebarLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(180, 0));
        groupBox->setMaximumSize(QSize(220, 16777215));
        menuButtonsLayout = new QVBoxLayout(groupBox);
        menuButtonsLayout->setSpacing(8);
        menuButtonsLayout->setObjectName(QString::fromUtf8("menuButtonsLayout"));
        menuButtonsLayout->setContentsMargins(10, 15, 10, 10);
        b_usuarios = new QPushButton(groupBox);
        b_usuarios->setObjectName(QString::fromUtf8("b_usuarios"));

        menuButtonsLayout->addWidget(b_usuarios);

        b_nodos = new QPushButton(groupBox);
        b_nodos->setObjectName(QString::fromUtf8("b_nodos"));

        menuButtonsLayout->addWidget(b_nodos);

        b_consulta = new QPushButton(groupBox);
        b_consulta->setObjectName(QString::fromUtf8("b_consulta"));

        menuButtonsLayout->addWidget(b_consulta);

        b_compus = new QPushButton(groupBox);
        b_compus->setObjectName(QString::fromUtf8("b_compus"));

        menuButtonsLayout->addWidget(b_compus);

        menuSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        menuButtonsLayout->addItem(menuSpacer);


        sidebarLayout->addWidget(groupBox);

        b_cerrarSesion = new QPushButton(centralwidget);
        b_cerrarSesion->setObjectName(QString::fromUtf8("b_cerrarSesion"));
        b_cerrarSesion->setMinimumSize(QSize(0, 45));

        sidebarLayout->addWidget(b_cerrarSesion);


        mainHorizontalLayout->addLayout(sidebarLayout);

        contentLayout = new QVBoxLayout();
        contentLayout->setSpacing(15);
        contentLayout->setObjectName(QString::fromUtf8("contentLayout"));
        main_label = new QLabel(centralwidget);
        main_label->setObjectName(QString::fromUtf8("main_label"));
        main_label->setAlignment(Qt::AlignCenter);

        contentLayout->addWidget(main_label);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page_usuarios = new QWidget();
        page_usuarios->setObjectName(QString::fromUtf8("page_usuarios"));
        usuariosLayout = new QHBoxLayout(page_usuarios);
        usuariosLayout->setSpacing(15);
        usuariosLayout->setObjectName(QString::fromUtf8("usuariosLayout"));
        usuariosActionsLayout = new QVBoxLayout();
        usuariosActionsLayout->setSpacing(10);
        usuariosActionsLayout->setObjectName(QString::fromUtf8("usuariosActionsLayout"));
        user_add = new QPushButton(page_usuarios);
        user_add->setObjectName(QString::fromUtf8("user_add"));

        usuariosActionsLayout->addWidget(user_add);

        user_delete = new QPushButton(page_usuarios);
        user_delete->setObjectName(QString::fromUtf8("user_delete"));

        usuariosActionsLayout->addWidget(user_delete);

        label_cambios = new QLabel(page_usuarios);
        label_cambios->setObjectName(QString::fromUtf8("label_cambios"));
        label_cambios->setAlignment(Qt::AlignCenter);

        usuariosActionsLayout->addWidget(label_cambios);

        user_change_pass = new QPushButton(page_usuarios);
        user_change_pass->setObjectName(QString::fromUtf8("user_change_pass"));

        usuariosActionsLayout->addWidget(user_change_pass);

        user_change_rank = new QPushButton(page_usuarios);
        user_change_rank->setObjectName(QString::fromUtf8("user_change_rank"));

        usuariosActionsLayout->addWidget(user_change_rank);

        verticalSpacer_usuarios = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        usuariosActionsLayout->addItem(verticalSpacer_usuarios);


        usuariosLayout->addLayout(usuariosActionsLayout);

        user_list = new QListWidget(page_usuarios);
        user_list->setObjectName(QString::fromUtf8("user_list"));

        usuariosLayout->addWidget(user_list);

        stackedWidget->addWidget(page_usuarios);
        page_nodos = new QWidget();
        page_nodos->setObjectName(QString::fromUtf8("page_nodos"));
        nodosLayout = new QVBoxLayout(page_nodos);
        nodosLayout->setObjectName(QString::fromUtf8("nodosLayout"));
        arduino_list = new QListWidget(page_nodos);
        arduino_list->setObjectName(QString::fromUtf8("arduino_list"));

        nodosLayout->addWidget(arduino_list);

        nodosButtonsLayout = new QHBoxLayout();
        nodosButtonsLayout->setObjectName(QString::fromUtf8("nodosButtonsLayout"));
        horizontalSpacer_nodos = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        nodosButtonsLayout->addItem(horizontalSpacer_nodos);

        horizontalSpacer_nodos2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        nodosButtonsLayout->addItem(horizontalSpacer_nodos2);


        nodosLayout->addLayout(nodosButtonsLayout);

        stackedWidget->addWidget(page_nodos);
        page_consulta = new QWidget();
        page_consulta->setObjectName(QString::fromUtf8("page_consulta"));
        consultaLayout = new QVBoxLayout(page_consulta);
        consultaLayout->setObjectName(QString::fromUtf8("consultaLayout"));
        data_list = new QListWidget(page_consulta);
        data_list->setObjectName(QString::fromUtf8("data_list"));

        consultaLayout->addWidget(data_list);

        stackedWidget->addWidget(page_consulta);
        page_compus = new QWidget();
        page_compus->setObjectName(QString::fromUtf8("page_compus"));
        compusMainLayout = new QVBoxLayout(page_compus);
        compusMainLayout->setObjectName(QString::fromUtf8("compusMainLayout"));
        verticalSpacer_compus_top = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        compusMainLayout->addItem(verticalSpacer_compus_top);

        compusGridLayout = new QGridLayout();
        compusGridLayout->setObjectName(QString::fromUtf8("compusGridLayout"));
        compusGridLayout->setHorizontalSpacing(40);
        compusGridLayout->setVerticalSpacing(40);
        pc1_layout = new QHBoxLayout();
        pc1_layout->setObjectName(QString::fromUtf8("pc1_layout"));
        horizontalSpacer_pc1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc1_layout->addItem(horizontalSpacer_pc1);

        pc1_button = new QPushButton(page_compus);
        pc1_button->setObjectName(QString::fromUtf8("pc1_button"));

        pc1_layout->addWidget(pc1_button);

        horizontalSpacer_pc1_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc1_layout->addItem(horizontalSpacer_pc1_2);


        compusGridLayout->addLayout(pc1_layout, 0, 0, 1, 1);

        pc2_layout = new QHBoxLayout();
        pc2_layout->setObjectName(QString::fromUtf8("pc2_layout"));
        horizontalSpacer_pc2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc2_layout->addItem(horizontalSpacer_pc2);

        pc2_button = new QPushButton(page_compus);
        pc2_button->setObjectName(QString::fromUtf8("pc2_button"));

        pc2_layout->addWidget(pc2_button);

        horizontalSpacer_pc2_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc2_layout->addItem(horizontalSpacer_pc2_2);


        compusGridLayout->addLayout(pc2_layout, 0, 1, 1, 1);

        pc3_layout = new QHBoxLayout();
        pc3_layout->setObjectName(QString::fromUtf8("pc3_layout"));
        horizontalSpacer_pc3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc3_layout->addItem(horizontalSpacer_pc3);

        pc3_button = new QPushButton(page_compus);
        pc3_button->setObjectName(QString::fromUtf8("pc3_button"));

        pc3_layout->addWidget(pc3_button);

        horizontalSpacer_pc3_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc3_layout->addItem(horizontalSpacer_pc3_2);


        compusGridLayout->addLayout(pc3_layout, 1, 0, 1, 1);

        pc4_layout = new QHBoxLayout();
        pc4_layout->setObjectName(QString::fromUtf8("pc4_layout"));
        horizontalSpacer_pc4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc4_layout->addItem(horizontalSpacer_pc4);

        pc4_button = new QPushButton(page_compus);
        pc4_button->setObjectName(QString::fromUtf8("pc4_button"));

        pc4_layout->addWidget(pc4_button);

        horizontalSpacer_pc4_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pc4_layout->addItem(horizontalSpacer_pc4_2);


        compusGridLayout->addLayout(pc4_layout, 1, 1, 1, 1);


        compusMainLayout->addLayout(compusGridLayout);

        verticalSpacer_compus_bottom = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        compusMainLayout->addItem(verticalSpacer_compus_bottom);

        stackedWidget->addWidget(page_compus);

        contentLayout->addWidget(stackedWidget);


        mainHorizontalLayout->addLayout(contentLayout);

        MenuWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MenuWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1000, 22));
        MenuWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MenuWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MenuWindow->setStatusBar(statusbar);

        retranslateUi(MenuWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MenuWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MenuWindow)
    {
        MenuWindow->setWindowTitle(QCoreApplication::translate("MenuWindow", "Frituinos - Sistema de Control", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MenuWindow", "\342\232\241 OPCIONES", nullptr));
        b_usuarios->setText(QCoreApplication::translate("MenuWindow", "\360\237\221\245 Usuarios", nullptr));
        b_nodos->setText(QCoreApplication::translate("MenuWindow", "\360\237\214\220 Nodos", nullptr));
        b_consulta->setText(QCoreApplication::translate("MenuWindow", "\360\237\223\212 Consulta", nullptr));
        b_compus->setText(QCoreApplication::translate("MenuWindow", "\360\237\222\273 Computadoras", nullptr));
        b_cerrarSesion->setText(QCoreApplication::translate("MenuWindow", "\360\237\232\252 Cerrar Sesi\303\263n", nullptr));
        main_label->setText(QCoreApplication::translate("MenuWindow", "Men\303\272 Principal", nullptr));
        user_add->setText(QCoreApplication::translate("MenuWindow", "\342\236\225 Agregar", nullptr));
        user_delete->setText(QCoreApplication::translate("MenuWindow", "\360\237\227\221\357\270\217 Eliminar", nullptr));
        label_cambios->setText(QCoreApplication::translate("MenuWindow", "\342\234\217\357\270\217 Cambios", nullptr));
        user_change_pass->setText(QCoreApplication::translate("MenuWindow", "\360\237\224\220 Contrase\303\261a", nullptr));
        user_change_rank->setText(QCoreApplication::translate("MenuWindow", "\342\255\220 Permisos", nullptr));
        pc1_button->setText(QCoreApplication::translate("MenuWindow", "\360\237\226\245\357\270\217\n"
"PC 1", nullptr));
        pc2_button->setText(QCoreApplication::translate("MenuWindow", "\360\237\226\245\357\270\217\n"
"PC 2", nullptr));
        pc3_button->setText(QCoreApplication::translate("MenuWindow", "\360\237\226\245\357\270\217\n"
"PC 3", nullptr));
        pc4_button->setText(QCoreApplication::translate("MenuWindow", "\360\237\226\245\357\270\217\n"
"PC 4", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MenuWindow: public Ui_MenuWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWINDOW_H
