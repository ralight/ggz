SUBDIRS = .

bin_PROGRAMS = grubby

grubby_SOURCES = guru.c main.c module.c
grubby_LDADD = -ldl $(LIB_GGZCORE)
grubby_LDFLAGS = $(GGZCORE_LDFLAGS)

lib_LTLIBRARIES = libguru_netggz.la libguru_i18n.la
noinst_LTLIBRARIES = libguru_player.la

libguru_netggz_la_SOURCES = net.c
libguru_netggz_la_LIBADD = $(LIB_GGZCORE)
libguru_netggz_la_LDFLAGS = $(GGZCORE_LDFLAGS)

libguru_i18n_la_SOURCES = i18n.c
libguru_i18n_la_LIBADD = libguru_player.la

libguru_player_la_SOURCES = player.c

noinst_HEADERS = guru.h module.h i18n.h gurumod.h net.h player.h

INCLUDES = $(GGZCORE_INCLUDES)

