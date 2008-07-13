## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

ggzexecmod_PROGRAMS = ggz.ttt3d

ggz_ttt3d_SOURCES = main.c display.c
ggz_ttt3d_LDFLAGS = $(GGZMOD_LDFLAGS)
ggz_ttt3d_LDADD = $(LIB_GGZMOD) $(SDL_LIBS) -lGL

noinst_HEADERS = display.h

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(SDL_CFLAGS)

ttt3ddir = $(ggzdatadir)/ttt3d
dist_ttt3d_DATA = board.bmp cursor.bmp figure.bmp lines.bmp

EXTRA_DIST = HOWTOPLAY

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

