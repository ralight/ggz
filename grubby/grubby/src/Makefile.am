SUBDIRS = .

bin_PROGRAMS = guru

guru_SOURCES = guru.c main.c module.c
guru_LDADD = -ldl $(LIB_GGZCORE)
guru_LDFLAGS = $(GGZCORE_LDFLAGS)

lib_LTLIBRARIES = libguru_netggz.la libguru_i18n.la

libguru_netggz_la_SOURCES = net.c
libguru_netggz_la_LIBADD = $(LIB_GGZCORE)
libguru_netggz_la_LDFLAGS = $(GGZCORE_LDFLAGS)

libguru_i18n_la_SOURCES = i18n.c

noinst_HEADERS = guru.h module.h i18n.h gurumod.h net.h player.h

INCLUDES = $(GGZCORE_INCLUDES)

