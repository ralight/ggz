lib_LTLIBRARIES = libggzsh.la
libggzsh_la_SOURCES = ggzsh.cpp
libggzsh_la_LIBADD = -lpython2.1

INCLUDES = -I/usr/include/python2.1

noinst_HEADERS = ggzsh.sh

bin_PROGRAMS = call
call_SOURCES = main.cpp
call_LDADD = libggzsh.la

#Call something like this:
#all:
#	g++ -o call main.cpp ggzsh.cpp -I/usr/include/python2.1 -lpython2.1

