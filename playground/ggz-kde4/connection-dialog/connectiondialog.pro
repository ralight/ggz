TEMPLATE = app
TARGET = connectiondialog

#INCLUDEPATH += .. /home/spillner/kde/installation/include
#LIBS += -L/home/spillner/kde/installation/lib -lkdecore -lkdeui ../kggzmod/libkggzmod.so

LIBS += -L/home/josef/sys/kde4/lib -lkio

HEADERS += connectiondialog.h qasyncpixmap.h
SOURCES += connectiondialog.cpp qasyncpixmap.cpp main.cpp
