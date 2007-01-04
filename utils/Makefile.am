## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8813 2007-01-04 22:52:10Z jdorje $

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

SUBDIRS = man $(METASERV_DIR) $(TELGGZ_DIR) $(CMDCLIENT_DIR) $(GGZCOMM_DIR)

EXTRA_DIST = README.GGZ

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
