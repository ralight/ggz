## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4660 2002-09-23 07:54:17Z jdorje $

SUBDIRS = .

# We can't use wildcards (*) here because it will break "make dist"!
default_DATA =	default/bomb.xpm	\
		default/captain.xpm	\
		default/colonel.xpm	\
		default/flag.xpm	\
		default/general.xpm	\
		default/lieutenant.xpm	\
		default/major.xpm	\
		default/marshall.xpm	\
		default/miner.xpm	\
		default/scout.xpm	\
		default/sergeant.xpm	\
		default/spy.xpm
defaultdir = $(ggzdatadir)/combat/pixmaps/default

EXTRA_DIST = $(default_DATA)
