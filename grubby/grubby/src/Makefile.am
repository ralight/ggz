## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

bin_PROGRAMS = grubby

grubby_SOURCES = guru.c main.c module.c
grubby_LDADD = -ldl $(LIB_GGZCORE)
grubby_LDFLAGS = $(GGZCORE_LDFLAGS)

grubbydir = $(libdir)/grubby/coremodules
grubby_LTLIBRARIES = libguru_netggz.la libguru_i18n.la libguru_player.la libguru_netconsole.la libguru_netirc.la

libguru_netggz_la_SOURCES = net.c
libguru_netggz_la_LIBADD = $(LIB_GGZCORE)
libguru_netggz_la_LDFLAGS = $(GGZCORE_LDFLAGS)

libguru_netconsole_la_SOURCES = netconsole.c

libguru_netirc_la_SOURCES = netirc.c

libguru_i18n_la_SOURCES = i18n.c

libguru_player_la_SOURCES = player.c
libguru_player_la_LDFLAGS = $(GGZCORE_LDFLAGS)
libguru_player_la_LIBADD = $(LIB_GGZCORE)

noinst_HEADERS = guru.h module.h i18n.h gurumod.h net.h player.h

AM_CPPFLAGS = $(GGZCORE_INCLUDES)

