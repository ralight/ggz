
SUBDIRS = .

INCLUDES = -I$(top_srcdir)/src/ggzcore @EASYSOCK_INC@

bin_PROGRAMS = ggz-txt

ggz_txt_SOURCES = 	input.c \
			input.h \
			main.c \
			output.c \
			output.h \
			server.c \
			server.h

ggz_txt_LDADD = $(top_builddir)/src/ggzcore/libggzcore.a @EASYSOCK_LIB@
