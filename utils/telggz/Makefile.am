SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = main.c net.c chat.c meta.c
telggz_LDADD = -L/usr/local/lib -lggzcore $(top_srcdir)/metaserv/minidom/libminidom.a
INCLUDES = -I/usr/local/include -I$(top_srcdir)/metaserv/minidom

