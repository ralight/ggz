TEMPLATE = app
TARGET = vencedor

INCLUDEPATH += /home/josef/sys/kde4/include
LIBS += -L/home/josef/sys/kde4/lib
LIBS += -lkggzcore -lkggzlib -lkdegames

HEADERS = vencedor.h
SOURCES = vencedor.cpp main.cpp
