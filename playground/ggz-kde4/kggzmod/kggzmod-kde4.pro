TEMPLATE = lib
TARGET = kggzmod

QT -= gui
QT += network

INCLUDEPATH += /home/spillner/kde/installation/include ..
LIBS += -L /home/spillner/kde/installation/lib -lkdecore

# Input
HEADERS = event.h \
	misc_private.h \
	module.h \
	module_private.h \
	player.h \
	player_private.h \
	request.h \
	statistics.h \
	statistics_private.h

SOURCES = event.cpp \
	module.cpp \
	player.cpp \
	request.cpp \
	statistics.cpp

