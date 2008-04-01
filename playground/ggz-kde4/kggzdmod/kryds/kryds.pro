TEMPLATE = app
TARGET = kryds

QT -= gui

INCLUDEPATH += ../..
LIBS += -L..
LIBS += -lkggzdmod

HEADERS = kryds.h
SOURCES = kryds.cpp main.cpp
