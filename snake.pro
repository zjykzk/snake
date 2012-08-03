TEMPLATE = app
TARGET = snake
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = .
CONFIG += qt

# Input
HEADERS += snake.h
SOURCES += snake.cc
INCLUDEPATH += ./include
LIBS += -L./lib -llua52
