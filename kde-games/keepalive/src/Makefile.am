SUBDIRS = .

ggzexecmod_PROGRAMS = keepalive

keepalive_SOURCES = main.cpp canvas.cpp login.cpp world.cpp win.cpp
keepalive_LDFLAGS = $(all_libraries)
keepalive_LDADD = -lqt

noinst_HEADERS = protocol.h canvas.h login.h world.h win.h

INCLUDES = -I$(qt_includes)

METASOURCES = AUTO

