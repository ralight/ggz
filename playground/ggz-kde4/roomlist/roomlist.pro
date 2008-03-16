TEMPLATE = app
TARGET = roomlist

INCLUDEPATH += ../playerlist

HEADERS += roomlist.h ../playerlist/qrecursivesortfilterproxymodel.h room.h roomdelegate.h roomtreeview.h modelview.h
SOURCES += roomlist.cpp ../playerlist/qrecursivesortfilterproxymodel.cpp room.cpp roomdelegate.cpp roomtreeview.cpp main.cpp
