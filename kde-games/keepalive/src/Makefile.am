SUBDIRS = .

ggzexecmod_PROGRAMS = keepalive

keepalive_SOURCES = main.cpp canvas.cpp login.cpp world.cpp win.cpp chatbox.cpp
keepalive_LDFLAGS = $(all_libraries)
keepalive_LDADD = $(LIB_QT)

noinst_HEADERS = protocol.h canvas.h login.h world.h win.h chatbox.h

INCLUDES = -I$(qt_includes)

METASOURCES = AUTO

