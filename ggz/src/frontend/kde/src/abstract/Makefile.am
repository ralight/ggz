ggzcorepp_includes = -I ../../ggzcore++/src
ggz_libraries = -L ../../ggzcore++/src/.libs -leasysock -lggzcore -lggzcorepp

noinst_LIBRARIES = libkggzabstract.a

libkggzabstract_a_SOURCES = \
	KGGZRoom.cpp \
	KGGZServer.cpp \
	KGGZCallback.cpp

noinst_HEADERS = \
	KGGZRoom.h \
	KGGZServer.h \
	KGGZCallback.h

INCLUDES = $(ggzcorepp_includes) -I ..
