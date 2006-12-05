TEMPLATE = app
TARGET = kconnectx
INCLUDEPATH += .. /home/kde-devel/kde/include
LIBS += -L/home/kde-devel/kde/lib -lkdecore -lkdeui ../kggzmod/libkggzmod.so

# Input
HEADERS += kconnectx.h opt.h proto.h win.h
SOURCES += kconnectx.cpp main.cpp opt.cpp proto.cpp win.cpp
#The following line was inserted by qt3to4
QT +=  qt3support 
