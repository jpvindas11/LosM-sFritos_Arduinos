QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../ClientManager/src/Users \
               ../ClientManager/src/FileSystem

SOURCES += \
    autenticatorqt.cpp \
    changepassdialog.cpp \
    confirmdeleteuserdialog.cpp \
    datamenumanager.cpp \
    newuserdialog.cpp \
    setrankdialog.cpp \
    userdataqt.cpp \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    arduinomenumanager.cpp \
    ../ClientManager/src/Users/Semaphore.cpp \
    ../ClientManager/src/Users/Thread.cpp \
    ../ClientManager/src/Users/AuthenticationServer.cpp \
    ../ClientManager/src/FileSystem/FileSystem.cpp \
    ../ClientManager/src/Users/arduinoSimulation.cpp \
    ../ClientManager/src/Users/Master.cpp \
    usermenumanager.cpp

HEADERS += \
    autenticatorqt.h \
    changepassdialog.h \
    confirmdeleteuserdialog.h \
    datamenumanager.h \
    mainwindow.h \
    menuwindow.h \
    newuserdialog.h \
    setrankdialog.h \
    userdataqt.h \
    arduinomenumanager.h \
    ../ClientManager/src/Users/AuthenticationServer.hpp \
    ../ClientManager/src/FileSystem/FileSystem.hpp \
    ../ClientManager/src/Users/arduinoSimulation.hpp \
    ../ClientManager/src/Users/Master.hpp \
    usermenumanager.h

FORMS += \
    changepassdialog.ui \
    confirmdeleteuserdialog.ui \
    mainwindow.ui \
    menuwindow.ui \
    newuserdialog.ui \
    setrankdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += /home/axel/Escritorio/LosM-sFritos_Arduinos/ClientManager/src/FileSystem
INCLUDEPATH += /home/axel/Escritorio/LosM-sFritos_Arduinos/ClientManager/src/Users

LIBS += -lsodium
