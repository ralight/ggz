## Process this file with automake to produce Makefile.in

SUBDIRS = config .

EXTRA_DIST = #config/ggzcards.dsc config/ggzcards.room

ggzdexecmod_PROGRAMS = ggzd.ggzcards
ggzdgame_DATA =
ggzdroom_DATA =

ggzd_ggzcards_SOURCES = ggz.c ggz.h \
		       main.c \
		       cards.c cards.h \
		       common.c common.h \
		       game.c game.h \
		       games.c games.h \
		       protocols.h


ggzd_ggzcards_LDADD = $(LDADD)

test: test-recursive
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
#	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
#	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

test-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) -s $(AM_MAKEFLAGS) $$local_target) \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) -s $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"
