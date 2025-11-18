QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lssh

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../ClientManager/Util
INCLUDEPATH += ../ClientManager/ssh

SOURCES += \
    changepassdialog.cpp \
    confirmdeleteuserdialog.cpp \
    datamenumanager.cpp \
    dialogmanagepc.cpp \
    newuserdialog.cpp \
    setrankdialog.cpp \
    userdataqt.cpp \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    arduinomenumanager.cpp \
    ../ClientManager/Util/Socket.cpp \
    ../ClientManager/Util/UDPSocket.cpp \
    ../ClientManager/ssh/ssh.cpp \
    usermenumanager.cpp

HEADERS += \
    changepassdialog.h \
    confirmdeleteuserdialog.h \
    datamenumanager.h \
    dialogmanagepc.h \
    mainwindow.h \
    menuwindow.h \
    newuserdialog.h \
    setrankdialog.h \
    userdataqt.h \
    arduinomenumanager.h \
    ../ClientManager/Util/Socket.hpp \
    ../ClientManager/Util/UDPSocket.hpp \
    ../ClientManager/Util/Messages.hpp \
    ../ClientManager/Util/User.hpp \
    ../ClientManager/Util/IPConstants.hpp \
    ../ClientManager/Util/ServerDiscover.hpp \
    ../ClientManager/ssh/ssh.hpp \
    usermenumanager.h

FORMS += \
    changepassdialog.ui \
    confirmdeleteuserdialog.ui \
    dialogmanagepc.ui \
    mainwindow.ui \
    menuwindow.ui \
    newuserdialog.ui \
    setrankdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lsodium
