TEMPLATE = lib
TARGET = kggzcore

QT -= gui

INCLUDEPATH += /home/josef/sys/include
INCLUDEPATH += ..
LIBS += -L/home/josef/sys/lib
LIBS += -lggzcore

HEADERS = \
	coreclient.h \
	coreclientbase.h \
	error.h \
	misc.h \
	room.h \
	roombase.h \
	table.h \
	player.h

SOURCES = \
	coreclient.cpp \
	coreclientbase.cpp \
	error.cpp \
	misc.cpp \
	room.cpp \
	roombase.cpp \
	table.cpp \
	player.cpp
