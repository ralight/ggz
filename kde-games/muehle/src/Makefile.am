SUBDIRS = ggzwrap ai

ggzexecmod_PROGRAMS = muehle

muehle_SOURCES = main.cpp toplevel.cpp board.cpp stone.cpp qweb.cpp qextpainter.cpp net.cpp qwebpath.cpp
muehle_LDFLAGS = $(all_libraries)
muehle_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF)
#muehle_CXXFLAGS = -ggdb3
INCLUDES = $(all_includes)

noinst_HEADERS = toplevel.h board.h stone.h qweb.h qextpainter.h net.h qwebpath.h

METASOURCES = AUTO

