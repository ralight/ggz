## Process this file with automake to produce Makefile.in

SUBDIRS = .

ggzdgame_DATA = *.dsc
ggzdroom_DATA = *.room

# JDS: is this necessary?
#ggzd_ggzcards_LDADD = $(LDADD)

test:
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

