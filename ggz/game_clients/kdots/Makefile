# Makefile for KDots
#
# This one is just converted from the old KGGZ one, and _is_ a quick and dirty solution.
# Makefile.am follow's soon...
#
# (C) 2001 Josef Spillner

KDELIBS=/usr/local/kde/lib
KDEINCS=/usr/local/kde/include
QTLIBS=/usr/local/qt/lib
QTINCS=/usr/local/qt/include
TARGETDIR=/usr/local

#################################################################################

# Internal variables
QUIET=@
PROGRESS=-n .
CXX=g++
CPPFLAGS=-c -I$(KDEINCS) -I$(QTINCS) $(CPP_TEMP) -I Dot
LDFLAGS=-L$(KDELIBS) -L$(QTLIBS)
DOT=Dot/dots.o Dot/vdots.o Dot/qdots.o Dot/qdots.moc.o
LIBS=-lkdeui -lkdecore -lqt $(DOT)
VERSION=0.0.1

SOURCES=kdots kdots_options kdots_about kdots_help


default: all
all: compile compile_moc link

dot:
	cd Dot; make; cd ..

dotclean: cd Dot; make clean; cd ..

compile:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		$(CXX) $(CPPFLAGS) $$i.cpp; \
		echo $(PROGRESS); \
	done
	$(QUIET) $(CXX) $(CPPFLAGS) main.cpp;
	$(QUIET) echo $(PROGRESS);
	$(QUIET) echo

compile_moc:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		$(QTDIR)/bin/moc -nw -o $$i.moc.cpp $$i.h; \
		$(CXX) $(CPPFLAGS) $$i.moc.cpp; \
		echo $(PROGRESS); \
	done; \
	echo;

link:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		OBJLIST=`echo -n $$OBJLIST $$i.o" "`; \
		MOCOBJLIST=`echo -n $$MOCOBJLIST $$i.moc.o" "`; \
        done; \
	$(CXX) $(LDFLAGS) $(LIBS) -o kdots main.o $$OBJLIST $$MOCOBJLIST $(LD_TEMP)

clean:
	$(QUIET) rm -rf *.o
	$(QUIET) rm -rf *.moc.cpp
	$(QUIET) rm -f kdots
