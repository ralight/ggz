# Makefile for KGGZ
# (C) 2000 Josef Spillner

# Anpassen!
KDEDIR=/usr/local/kde
QTDIR=/usr/local/qt
TARGETDIR=/usr/local

# Das müßte so stimmen :-)
QUIET=@
PROGRESS=-n .
SHELL=/bin/sh
CXX=g++
CPPFLAGS=-c -I$(KDEDIR)/include -I$(QTDIR)/include -fno-rtti
LDFLAGS=-L$(KDEDIR)/lib -L$(QTDIR)/lib
LIBS=-lkdeui -lkdecore -lqt -ldl -lggzcore -lDCOP
LD_TEMP=/usr/local/lib/libggzcore.a

# Quellen zum Compilieren
SOURCES=kggz kggz_roomsmenu \
	kggz_chat kggz_startup kggz_tables kggz_users \
	kggz_chatwidget \
	kggz_preferences kggz_settings kggz_profiles kggz_connect kggz_launch \
	kggz_server

# Rules
all:
	$(QUIET) echo KGGZ
	$(QUIET) echo Compilieren der Quellen:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		$(CXX) $(CPPFLAGS) $$i.cpp; \
		echo $(PROGRESS); \
	done
	$(QUIET) $(CXX) $(CPPFLAGS) main.cpp;
	$(QUIET) echo $(PROGRESS);
	$(QUIET) echo

	$(QUIET) echo Ausführen des Moc:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		OBJLIST=`echo -n $$OBJLIST $$i.o" "`; \
		MOCOBJLIST=`echo -n $$MOCOBJLIST $$i.moc.o" "`; \
		$(QTDIR)/bin/moc -nw -o $$i.moc.cpp $$i.h; \
		$(CXX) $(CPPFLAGS) $$i.moc.cpp; \
		echo $(PROGRESS); \
	done; \
	echo; \
	echo Linken:; \
	$(CXX) $(LDFLAGS) $(LIBS) -o kggz main.o $$OBJLIST $$MOCOBJLIST $(LD_TEMP)

	$(QUIET) echo Fertig. Nun als root "'make install'" aufrufen!

clean:
	$(QUIET) echo Aufräumen:
	$(QUIET) rm -rf *.o
	$(QUIET) rm -rf *.moc.cpp
	$(QUIET) rm -f kggz
	$(QUIET) echo Fertig.
