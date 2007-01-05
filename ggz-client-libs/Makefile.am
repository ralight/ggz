## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8831 2007-01-05 00:51:25Z jdorje $

ggzwrap_dirs = ggzwrap ggz-wrapper
if COND_GGZWRAP
ggzwrap_subdirs = ggzwrap ggz-wrapper
endif

base_subdirs = ggzmod-ggz ggzcore ggzmod ggz-config man tests desktop po

SUBDIRS = $(base_subdirs) $(ggzwrap_subdirs)
DIST_SUBDIRS = $(base_subdirs) $(ggzwrap_dirs)

EXTRA_DIST = README.GGZ QuickStart.GGZ indent.pro

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz

