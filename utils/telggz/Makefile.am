SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = main.c net.c chat.c meta.c
telggz_LDADD = -L/usr/local/lib -lggzcore $(top_srcdir)/metaserv/minidom/libminidom.a
INCLUDES = -I/usr/local/include -I$(top_srcdir)/metaserv/minidom

# TODO: convert to acinclude.ggz

#all:
#	gcc -Wall -c main.c -I/usr/local/include
#	gcc -Wall -c net.c -I/usr/local/include
#	gcc -Wall -c chat.c -I/usr/local/include
#	gcc -o telggz main.o net.o chat.o -L/usr/local/lib -lggzcore

