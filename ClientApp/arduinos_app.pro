QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    ../ClientManager/src/FileSystem/FileSystem.cpp \
    ../ClientManager/src/Users/AuthenticationServer.cpp \
    ../ClientManager/src/Users/Semaphore.cpp \
    ../ClientManager/src/Users/Thread.cpp \



HEADERS += \
    mainwindow.h \
    menuwindow.h

FORMS += \
    mainwindow.ui \
    menuwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += /home/axel/Escritorio/LosM-sFritos_Arduinos/ClientManager/src/FileSystem
INCLUDEPATH += /home/axel/Escritorio/LosM-sFritos_Arduinos/ClientManager/src/Users

LIBS += -lsodium

