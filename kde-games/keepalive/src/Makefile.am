SUBDIRS = .

ggzexecmod_PROGRAMS = keepalive

keepalive_SOURCES = main.cpp canvas.cpp login.cpp world.cpp win.cpp chatbox.cpp network.cpp animdialog.cpp unitfactory.cpp
keepalive_LDFLAGS = $(GGZMOD_LDFLAGS) $(all_libraries)
keepalive_LDADD = $(LIB_GGZMOD) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)

noinst_HEADERS = protocol.h canvas.h login.h world.h win.h chatbox.h network.h animdialog.h unitfactory.h

INCLUDES = $(GGZMOD_INCLUDES) $(all_includes)

METASOURCES = AUTO

