SUBDIRS = .

ggzexecmod_PROGRAMS = guru-chess

guru_chess_SOURCES = main.c
guru_chess_LDFLAGS = $(GGZMOD_LDFLAGS)
guru_chess_LDADD = $(top_builddir)/games/lib/libggzpassive.a $(LIB_GGZMOD)

EXTRA_DIST = module.dsc.in

INCLUDES = -I $(top_srcdir)/games/lib $(LIBGGZ_INCLUDES)

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

