#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T20:20:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mg_test
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    web_server.cpp \
    mongoose.c

HEADERS  += widget.h \
    mongoose.h \
    web_server.h

FORMS    += widget.ui
