TEMPLATE = app
TARGET = connectiondialog

INCLUDEPATH += /home/josef/home.checkout/projects/ggz-trunk/kde-center/kggzlib
LIBS += -L/home/josef/home.checkout/projects/ggz-trunk/kde-center/BUILD/lib -lkggzlib -L/home/josef/sys/kde4/lib -lkio

SOURCES += main.cpp
