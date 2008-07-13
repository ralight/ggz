TEMPLATE = app
TARGET = kryds

QT -= gui

# QT_NO_DEBUG: silences kDebug, on by default
# QT_NO_DEBUG_OUTPUT: silences qDebug, off by default
#DEFINES += QT_NO_DEBUG_OUTPUT
CONFIG += debug

INCLUDEPATH += ../..
INCLUDEPATH += /home/josef/sys/kde4/include
LIBS += -L..
LIBS += -lkggzdmod
LIBS += -L/home/josef/sys/kde4/lib
LIBS += -lkdecore -lkggznet

HEADERS = kryds.h tictactoe_server.h
SOURCES = kryds.cpp tictactoe_server.cpp main.cpp
