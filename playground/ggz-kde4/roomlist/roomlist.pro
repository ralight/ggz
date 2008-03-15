TEMPLATE = app
TARGET = roomlist

INCLUDEPATH += ../playerlist

HEADERS += roomlist.h ../playerlist/qrecursivesortfilterproxymodel.h room.h
SOURCES += roomlist.cpp ../playerlist/qrecursivesortfilterproxymodel.cpp room.cpp main.cpp
