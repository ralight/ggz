## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3826 2002-04-07 10:48:03Z dr_maux $

# This should contain all possible subdirs, so that they're all distributed
# no matter which are actually selected.  Of course, so long as they're all
# selected by default it shouldn't generally be a problem.
DIST_SUBDIRS = man metaserv telggz cmd-client

# Our subdirs are decided by configure.
SUBDIRS = @SUBDIRS@

clean-local:
	-rm -f config.cache

