SUBDIRS = .

grubbydir = $(libdir)/grubby/modules
grubby_LTLIBRARIES = \
	libgurumod_simplereturn.la \
	libgurumod_programming.la \
	libgurumod_exec.la \
	libgurumod_badword.la \
	libgurumod_self.la \
	libgurumod_people.la \
	libgurumod_learning.la \
	libgurumod_extra.la \
	libgurumod_embed.la

libgurumod_simplereturn_la_SOURCES = simplereturn.c

libgurumod_programming_la_SOURCES = programming.c

libgurumod_exec_la_SOURCES = exec.c
libgurumod_exec_la_LIBADD = $(LIB_GGZCORE)
libgurumod_exec_la_LDFLAGS = $(GGZCORE_LDFLAGS)

libgurumod_badword_la_SOURCES = badword.c
libgurumod_badword_la_LIBADD = $(LIB_GGZCORE)
libgurumod_badword_la_LDFLAGS = $(GGZCORE_LDFLAGS)

libgurumod_self_la_SOURCES = self.c

libgurumod_embed_la_SOURCES = embed.c
libgurumod_embed_la_LIBADD = $(ruby_libraries) $(perl_libraries)

libgurumod_extra_la_SOURCES = extra.c

libgurumod_people_la_SOURCES = people.c
libgurumod_people_la_LDFLAGS = $(GGZCORE_LDFLAGS)

INCLUDES = -I$(top_srcdir)/grubby/src $(GGZCORE_INCLUDES) $(ruby_includes) $(perl_includes)

libgurumod_learning_la_SOURCES = learning.c

# noinst_HEADERS = 
