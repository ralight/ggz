ggzexecmod_PROGRAMS = guru-ttt

guru_ttt_SOURCES = main.c
guru_ttt_LDFLAGS = $(GGZMOD_LDFLAGS)
guru_ttt_LDADD = $(top_builddir)/games/lib/libggzpassive.a $(LIB_GGZMOD)

EXTRA_DIST = module.dsc.in

AM_CPPFLAGS = -I $(top_srcdir)/games/lib $(LIBGGZ_INCLUDES)

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

