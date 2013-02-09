INCLUDEPATH *= \
    sundown/src \
    sundown/html

HEADERS *= \
    sundown/src/markdown.h \
    sundown/src/buffer.h \
    sundown/src/stack.h \
    sundown/src/autolink.h \
    sundown/html/html.h \
    sundown/html/houdini.h

SOURCES *= \
    sundown/src/markdown.c \
    sundown/src/buffer.c \
    sundown/src/stack.c \
    sundown/src/autolink.c \
    sundown/html/html.c \
    sundown/html/houdini_href_e.c \
    sundown/html/houdini_html_e.c
