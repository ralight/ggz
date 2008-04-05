ggzwrap_dirs = ggzwrap ggz-wrapper
if COND_GGZWRAP
ggzwrap_subdirs = ggzwrap ggz-wrapper
endif

base_subdirs = ggzmod-ggz ggzcore ggzmod ggz-config man tests desktop po

SUBDIRS = $(base_subdirs) $(ggzwrap_subdirs)
DIST_SUBDIRS = $(base_subdirs) $(ggzwrap_dirs)

EXTRA_DIST = README.GGZ QuickStart.GGZ indent.pro

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz

pkgconfigdir = $(datadir)/pkgconfig
pkgconfig_DATA = ggzmod.pc ggzcore.pc
