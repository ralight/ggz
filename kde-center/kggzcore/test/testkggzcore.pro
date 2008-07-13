TEMPLATE = app
TARGET = testkggzcore

QT -= gui

INCLUDEPATH += ../..
LIBS += -L..
LIBS += -lkggzcore

HEADERS = testkggzcore.h
SOURCES = testkggzcore.cpp main.cpp
