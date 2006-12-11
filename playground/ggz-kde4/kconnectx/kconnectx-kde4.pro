TEMPLATE = app
TARGET = kconnectx

QT += network

INCLUDEPATH += .. /home/spillner/kde/installation/include
LIBS += -L/home/spillner/kde/installation/lib -lkdecore -lkdeui ../kggzmod/libkggzmod.so

HEADERS += kconnectx.h opt.h proto.h win.h
SOURCES += kconnectx.cpp main.cpp opt.cpp proto.cpp win.cpp
