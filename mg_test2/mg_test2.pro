#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T22:57:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mg_test2
TEMPLATE = app

CONFIG += console

DEFINES += NS_ENABLE_DEBUG
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += main.cpp\
        widget.cpp \
    mongoose.cpp

HEADERS  += widget.h \
    mongoose.h

FORMS    += widget.ui
