SUBDIRS = .

ggzexecmod_PROGRAMS = guru-ttt

guru_ttt_SOURCES = main.c
guru_ttt_LDFLAGS = $(GGZMOD_LDFLAGS)
guru_ttt_LDADD = $(LIB_GGZMOD)

INCLUDES = $(GGZMOD_INCLUDES) $(SDL_CFLAGS)

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

