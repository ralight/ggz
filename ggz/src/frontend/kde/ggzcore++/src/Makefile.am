SUBDIRS = .

libggzcorepp_la_SOURCES = GGZCore.cpp \
			GGZCoreConf.cpp \
			GGZCoreConfio.cpp \
			GGZCoreGame.cpp \
			GGZCoreGametype.cpp \
			GGZCoreModule.cpp \
			GGZCorePlayer.cpp \
			GGZCoreRoom.cpp \
			GGZCoreServer.cpp \
			GGZCoreTable.cpp

include_HEADERS =       GGZCore.h \
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

lib_LTLIBRARIES =       libggzcorepp.la

libggzcorepp_la_LDFLAGS = -version-info 0:1:0 @GGZCORE_LDFLAGS@ @LIB_GGZCORE@
