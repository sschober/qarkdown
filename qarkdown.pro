TEMPLATE = app
TARGET = qarkdown

CONFIG -= release debug debug_and_release
QT *= core gui webkit

RESOURCES *= ressources.qrc

FORMS *= mainwindow.ui

HEADERS *= mainwindow.h \
    hscrollwebview.h \
    pmh_parser.h \
    pmh_definitions.h \
    highlighter.h

SOURCES *= main.cpp \
    mainwindow.cpp \
    hscrollwebview.cpp \
    pmh_parser.c \
    highlighter.cpp

OTHER_FILES *= README.md

exists( sundown ) {
    include( sundown.pri )
} else {
    LIBS *= -lsundown
}
