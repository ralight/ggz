SUBDIRS = .

noinst_LTLIBRARIES = libggzcorepp.la

libggzcorepp_la_SOURCES = \
			GGZCore.cpp \
			GGZCoreConf.cpp \
			GGZCoreConfio.cpp \
			GGZCoreGame.cpp \
			GGZCoreGametype.cpp \
			GGZCoreModule.cpp \
			GGZCorePlayer.cpp \
			GGZCoreRoom.cpp \
			GGZCoreServer.cpp \
			GGZCoreTable.cpp \
			GGZCoreCommon.cpp

include_HEADERS = \
			GGZCore.h \
			GGZCoreConf.h \
			GGZCoreConfio.h \
			GGZCoreGame.h \
			GGZCoreGametype.h \
			GGZCoreModule.h \
			GGZCorePlayer.h \
			GGZCoreRoom.h \
			GGZCoreServer.h \
			GGZCoreTable.h \
			GGZCoreCommon.h

libggzcorepp_la_LDFLAGS = -version-info 2:0:0 $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS)
libggzcorepp_la_LIBADD = $(LIB_GGZCORE) $(LIB_GGZ)

AM_CXXFLAGS = @COMPILEOPTIONS@

INCLUDES = $(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES)

api-docs:
	-kdoc -d ../doc/html -n GGZCore++ *.cpp *.h

