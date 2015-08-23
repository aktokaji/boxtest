#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T13:23:24
#
#-------------------------------------------------

QT       -= gui

TARGET = tlsdll
TEMPLATE = lib

DESTDIR = E:\testbed

DEFINES += TLSDLL_LIBRARY

SOURCES += tlsdll.cpp

HEADERS += tlsdll.h\
        tlsdll_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
