SUBDIRS = .

bin_PROGRAMS = guru

guru_SOURCES = guru.c main.c module.c
guru_LDADD = -ldl -L/usr/local/lib -lggzcore

#gi18n_SOURCES = i18n.c test_i18n.c
#gi18n_LDADD =

#gplayer_SOURCES = player.c test_player.c
#gplayer_LDADD = -lggzcore

lib_LTLIBRARIES = libguru_netggz.la

libguru_netggz_la_SOURCES = net.c
libguru_netggz_la_LIBADD = -lggzcore
libguru_netggz_la_LDFLAGS = -L/usr/local/lib

#gnet_SOURCES = test_net.c
#gnet_LDADD = libguru_netggz.la
#gnet_LDFLAGS = -ldl

noinst_HEADERS = guru.h module.h i18n.h gurumod.h net.h player.h


#all:
#	gcc -Wall -c guru.c
#	gcc -Wall -c main.c
#	gcc -Wall -c module.c
#	gcc -o guru main.o guru.o module.o -ldl
#	gcc -Wall -c i18n.c
#	gcc -Wall -c test_i18n.c
#	gcc -o guru_i18n i18n.o test_i18n.o
#
#clean:
#	rm -f guru
#	rm -f guru_i18n
#	rm -f *.o

