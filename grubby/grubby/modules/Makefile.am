## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8774 2006-12-28 14:25:34Z oojah $

SUBDIRS = .

grubbydir = $(libdir)/grubby/modules
grubby_LTLIBRARIES = \
	libgurumod_badword.la \
#	libgurumod_banner.la \
	libgurumod_embed.la \
	libgurumod_exec.la \
	libgurumod_extra.la \
	libgurumod_game.la \
	libgurumod_learning.la \
	libgurumod_people.la \
	libgurumod_programming.la \
	libgurumod_self.la \
	libgurumod_simplereturn.la \
	libgurumod_time.la

AM_CPPFLAGS = -I$(top_srcdir)/grubby/src $(GGZCORE_INCLUDES) \
	$(ruby_includes) $(perl_includes) $(python_includes) $(tcl_includes) $(php_includes)

noinst_HEADERS = game.h

libgurumod_badword_la_SOURCES = badword.c
libgurumod_badword_la_LIBADD = $(LIB_GGZCORE)
libgurumod_badword_la_LDFLAGS = -module -avoid-version $(GGZCORE_LDFLAGS)

#libgurumod_banner_la_SOURCES = banner.c
#libgurumod_banner_la_LIBADD = $(mysql_libraries)
#libgurumod_banner_la_LDFLAGS = -module -avoid-version

libgurumod_embed_la_SOURCES = embed.c
libgurumod_embed_la_LIBADD = $(ruby_libraries) $(perl_libraries) $(python_libraries) $(tcl_libraries) $(php_libraries)
libgurumod_embed_la_LDFLAGS = -module -avoid-version

libgurumod_exec_la_SOURCES = exec.c
libgurumod_exec_la_LIBADD = $(LIB_GGZCORE)
libgurumod_exec_la_LDFLAGS = -module -avoid-version $(GGZCORE_LDFLAGS)

libgurumod_extra_la_SOURCES = extra.c
libgurumod_extra_la_LDFLAGS = -module -avoid-version

libgurumod_game_la_SOURCES = game.c
libgurumod_game_la_LDFLAGS = -module -avoid-version

libgurumod_learning_la_SOURCES = learning.c
libgurumod_learning_la_LDFLAGS = -module -avoid-version

libgurumod_people_la_SOURCES = people.c
libgurumod_people_la_LDFLAGS = -module -avoid-version $(GGZCORE_LDFLAGS)

libgurumod_programming_la_SOURCES = programming.c
libgurumod_programming_la_LDFLAGS = -module -avoid-version

libgurumod_self_la_SOURCES = self.c
libgurumod_self_la_LDFLAGS = -module -avoid-version

libgurumod_simplereturn_la_SOURCES = simplereturn.c
libgurumod_simplereturn_la_LDFLAGS = -module -avoid-version

libgurumod_time_la_SOURCES = time.c
libgurumod_time_la_LDFLAGS = -module -avoid-version

