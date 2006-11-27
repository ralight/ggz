noinst_LTLIBRARIES = libkggzmod.la
libkggzmod_la_SOURCES = event.cpp request.cpp player.cpp statistics.cpp module.cpp
libkggzmod_la_LDFLAGS = $(all_libraries)
libkggzmod_la_LIBADD = $(LIB_KDECORE)

noinst_HEADERS = event.h request.h player.h statistics.h module.h statistics_private.h player_private.h module_private.h

AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes)

METASOURCES = AUTO

