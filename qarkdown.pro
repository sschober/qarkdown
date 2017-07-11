TEMPLATE = app
TARGET = qarkdown

CONFIG -= release debug debug_and_release
QT *= core gui widgets webenginewidgets

RESOURCES *= ressources.qrc

FORMS *= mainwindow.ui \
    settings.ui

HEADERS *= mainwindow.h \
    hscrollwebview.h \
    pmh_parser.h \
    pmh_definitions.h \
    highlighter.h \
    plaintexteditor.h

SOURCES *= main.cpp \
    mainwindow.cpp \
    hscrollwebview.cpp \
    pmh_parser.c \
    highlighter.cpp \
    plaintexteditor.cpp

OTHER_FILES *= README.md

exists( hoedown ) {
    include( hoedown.pri )
} else {
    LIBS *= -lhoedown
}
