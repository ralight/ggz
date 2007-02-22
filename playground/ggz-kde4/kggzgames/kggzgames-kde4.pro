TEMPLATE = lib
TARGET = kggzgames

QT += network

INCLUDEPATH += /home/josef/kde/installation/include ..
LIBS += -L /home/josef/kde/installation/lib -lkdeui

# Input
HEADERS = kggzpacket.h kggzseatsdialog.h

SOURCES = kggzpacket.cpp kggzseatsdialog.cpp

