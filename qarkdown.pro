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
    hscrollwebview.cpp

HEADERS  += mainwindow.h \
    hscrollwebview.h

FORMS    += mainwindow.ui

LIBS    += -lsundown

OTHER_FILES += \
    README.md

RESOURCES += \
    ressources.qrc
