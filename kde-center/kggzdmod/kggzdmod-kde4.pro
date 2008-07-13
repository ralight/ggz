TEMPLATE = lib
TARGET = kggzdmod

QT -= gui

CONFIG += debug

INCLUDEPATH += /home/josef/sys/kde4/include
INCLUDEPATH += /home/josef/sys/include
INCLUDEPATH += ..
LIBS += -L/home/josef/sys/kde4/lib
LIBS += -L/home/josef/sys/lib
LIBS += -lkdecore
LIBS += -lggzdmod

HEADERS = event.h \
	event_private.h \
	module.h \
	module_private.h \
	player.h \
	player_private.h \
	request.h

SOURCES = event.cpp \
	module.cpp \
	player.cpp \
	request.cpp

