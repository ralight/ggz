SUBDIRS = .

lib_LTLIBRARIES = libggzsh.la
libggzsh_la_SOURCES = ggzsh.cpp
libggzsh_la_LIBADD = -lpython2.2 -ruby1.7

AM_CPPFLAGS = -I/usr/include/python2.2 -I/usr/lib/ruby/1.7/i386-linux

noinst_HEADERS = ggzsh.sh

bin_PROGRAMS = call
call_SOURCES = main.cpp
call_LDADD = libggzsh.la

#Call something like this:
#all:
#	g++ -o call main.cpp ggzsh.cpp -I/usr/include/python2.2 -I /usr/lib/ruby/1.7/i386-linux/ -lpython2.2 -lruby1.7
