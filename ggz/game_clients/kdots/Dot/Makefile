MOC=/usr/local/qt/bin/moc
QTINCS=-I /usr/local/qt/include

all:
	g++ -o dottest test.cpp dots.cpp

	$(MOC) -o qdots.moc.cpp qdots.h
	g++ -c dots.cpp
	g++ -c vdots.cpp
	g++ $(QTINCS) -c qdots.cpp
	g++ $(QTINCS) -c qdots.moc.cpp

clean:
	rm -f *.o
	rm -f *.moc.cpp
	rm -f dottest
