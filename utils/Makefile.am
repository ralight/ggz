if COND_METASERV
METASERV_DIR = metaserv
endif
if COND_TELGGZ
TELGGZ_DIR = telggz
endif
if COND_CMDCLIENT
CMDCLIENT_DIR = cmd-client
endif
if COND_GGZCOMM
GGZCOMM_DIR = ggzcomm
endif

SUBDIRS = man motds ggz-wrapper $(METASERV_DIR) $(TELGGZ_DIR) $(CMDCLIENT_DIR) $(GGZCOMM_DIR) po

if COND_GGZWRAP
SUBDIRS += ggzwrap
endif

EXTRA_DIST = README.GGZ

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
