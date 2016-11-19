#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T10:47:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = x3f_wrapper
TEMPLATE = app
CONFIG += static

SOURCES += main.cpp\
        mainwindow.cpp \
    cpreferencespane.cpp \
    stringconstants.cpp \
    cprocessingthread.cpp

HEADERS  += mainwindow.h \
    cpreferencespane.h \
    stringconstants.h \
    cprocessingthread.h
