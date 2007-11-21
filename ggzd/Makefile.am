if COND_COMPILE_GAMES
GAMES = game_servers
endif
if COND_COMPILE_SERVER
SERVER = ggzd
endif

SUBDIRS = ggzdmod ggzdmod++ $(SERVER) $(GAMES) man tests

CLEAN_FILES = ggzd.debug ggzd.log

EXTRA_DIST = README.GGZ m4/test.mk

include $(top_srcdir)/m4/test.mk

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
