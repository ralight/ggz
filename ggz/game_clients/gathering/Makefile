QTLIBS=/usr/local/qt/lib
QTINCS=/usr/local/qt/include
MOC=/usr/local/qt/bin/moc
NETMAPINCS=netmap-copy
NETMAPLIBS=netmap-copy

CXX=g++
CPPFLAGS=-c -I$(QTINCS) -I$(NETMAPINCS)
LDFLAGS=-L$(QTLIBS) -L$(NETMAPLIBS)
LIBS=-lqt -lnetmap

OBJLIST=main.o qnetmap.o qnetmapsample.o gathering.o
MOCLIST=qnetmap.moc.o qnetmapsample.moc.o gathering.moc.o

default: all

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

%.moc.o: %.moc.cpp
	foo

%.moc.cpp: %.h
	$(MOC) -o $@ $<

all: gathering

gathering: $(OBJLIST) $(MOCLIST)
	g++ -o gathering $(OBJLIST) $(MOCLIST) $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o
	rm -f gathering
