## Process this file with automake to produce Makefile.in

SUBDIRS = .

ggzdgame_DATA = *.dsc
ggzdroom_DATA = *.room

# Hmmm, does the above work or is the below necessary? --JDS

#ggzdgame_DATA = ggzcards.dsc \
#		ggzcards-bridge.dsc ggzcards-hearts.dsc \
#		ggzcards-lapocha.dsc ggzcards-spades.dsc \
#		ggzcards-suaro.dsc
#ggzdroom_DATA = ggzcards.room \
#		ggzcards-bridge.room ggzcards-hearts.room \
#		ggzcards-lapocha.room ggzcards-spades.room \
#		ggzcards-suaro.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

# JDS: is this necessary?
#ggzd_ggzcards_LDADD = $(LDADD)

test:
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

