bin_PROGRAMS = ggz.ttt3d

ggz_ttt3d_SOURCES = main.c display.c
ggz_ttt3d_LDFLAGS = $(GGZMOD_LDFLAGS)
ggz_ttt3d_LDADD = -lGL -lSDL $(LIB_GGZMOD)

noinst_HEADERS = display.h

ttt3ddir = $(ggzdatadir)/ttt3d
ttt3d_DATA = board.bmp cursor.bmp figure.bmp lines.bmp

EXTRA_DIST = HOWTOPLAY

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

