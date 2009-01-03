TEMPLATE = app
TARGET = table-view

INCLUDEPATH += /home/josef/sys/kde4/include
LIBS += -L/home/josef/sys/kde4/lib -lkio -lkggzcore -lkggzlib

HEADERS = kggztableview.h

SOURCES += main.cpp kggztableview.cpp
