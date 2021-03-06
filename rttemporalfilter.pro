#-------------------------------------------------
#
# Project created by QtCreator 2015-08-26T07:46:18
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = rttemporalfilter
DESTDIR = bin
OBJECTS_DIR = build
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
SOURCES += main.cpp \
    rttemporalfilter.cpp get_command_line_params.cpp

HEADERS += \
    rttemporalfilter.h get_command_line_params.h

INCLUDEPATH += mdaio
DEPENDPATH += mdaio
VPATH += mdaio
HEADERS += mdaio.h mda.h usagetracking.h
SOURCES += mdaio.cpp mda.cpp usagetracking.cpp
