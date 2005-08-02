## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

ggzexecmod_PROGRAMS = tttxt

tttxt_SOURCES = main.c

tttxt_LDADD = $(LIB_GGZMOD) $(LIB_GGZ)
tttxt_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc
