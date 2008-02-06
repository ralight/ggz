bin_PROGRAMS = ggz-ircgate

ggz_ircgate_SOURCES = main.c netggz.c netirc.c
ggz_ircgate_LDADD = $(LIB_GGZCORE) $(LIB_GETTEXT)
ggz_ircgate_LDFLAGS = $(GGZCORE_LDFLAGS)

noinst_HEADERS = net.h

AM_CPPFLAGS = $(GGZCORE_INCLUDES) -DPREFIX=\"$(prefix)\"
