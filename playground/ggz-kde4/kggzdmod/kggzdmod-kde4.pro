TEMPLATE = lib
TARGET = kggzdmod

QT -= gui

INCLUDEPATH += /home/josef/kde/installation/include
INCLUDEPATH += /home/josef/ggz/installation/include
INCLUDEPATH += ..
LIBS += -L/home/josef/kde/installation/lib
LIBS += -L/home/josef/ggz/installation/lib
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

