#-------------------------------------------------
#
# Project created by QtCreator 2012-05-14T21:08:10
#
#-------------------------------------------------

QT       += core gui webkit

TARGET = qarkdown
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hscrollwebview.cpp \
    pmh_parser.c \
    highlighter.cpp

HEADERS  += mainwindow.h \
    hscrollwebview.h \
    pmh_parser.h \
    pmh_definitions.h \
    highlighter.h

FORMS    += mainwindow.ui

LIBS    += -lsundown

OTHER_FILES += \
    README.md

RESOURCES += \
    ressources.qrc
