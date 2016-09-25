#-------------------------------------------------
#
# Project created by QtCreator 2016-09-16T14:22:08
#
#-------------------------------------------------

QT       += core gui

# CONFIG  += CONSOLE

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WinRegWriteBlocker
TEMPLATE = app

SOURCES += main.cpp \
    writeblockerengine.cpp \
    registrymonitor.cpp \
    systrayview.cpp \
    engineexception.cpp

HEADERS  += \
    writeblockerengine.h \
    registrymonitor.h \
    systrayview.h \
    writeblockerenginelistener.h \
    writeblockereventsource.h \
    engineexception.h
FORMS    +=

RESOURCES += \
    ressources.qrc

DISTFILES += \
    manifest.xml \
    ressources.rc

RC_FILE = ressources.rc

