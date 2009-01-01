noinst_LIBRARIES = libstackpath.a

libstackpath_a_SOURCES = \
	pathitem.cpp \
	pathitem.h \
	cwpathitem.cpp \
	cwpathitem.h

test:
	-g++ -Wall -c main.cpp
	-g++ -o teststackpath main.o libstackpath.a

