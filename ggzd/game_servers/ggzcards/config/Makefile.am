## Process this file with automake to produce Makefile.in

SUBDIRS = .

# the other dsc and room files are disabled because they don't work yet
ggzdgame_DATA = ggzcards.dsc
ggzdroom_DATA = ggzcards.room

# JDS: is this necessary?
#ggzd_ggzcards_LDADD = $(LDADD)

test:
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

