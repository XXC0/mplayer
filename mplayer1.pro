#-------------------------------------------------
#
# Project created by QtCreator 2019-09-05T10:54:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mplayer1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    lyc.cpp \
    gb2312_ucs2.cpp

HEADERS  += widget.h \
    lrc.h \
    gb2312_ucs2.h

RESOURCES += \
    Image.qrc
