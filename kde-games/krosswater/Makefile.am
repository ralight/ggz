SUBDIRS = zoneclient gfx

ggzexecmod_PROGRAMS = krosswater_client

krosswater_client_SOURCES = \
	main.cpp \
	krosswater.cpp \
	krosswater_move.cpp \
	dlg_person.cpp \
	dlg_about.cpp \
	dlg_help.cpp \
	dlg_again.cpp \
	qcw.cpp

krosswater_client_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS)
krosswater_client_LDADD = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_GGZ) zoneclient/libzoneclient.la

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) -I $(top_srcdir)/krosswater/zoneclient

noinst_HEADERS = \
	krosswater.h \
	krosswater_move.h \
	dlg_person.h \
	dlg_about.h \
	dlg_help.h \
	dlg_again.h \
	qcw.h

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

KDE_ICON = krosswater

DISTCLEANFILES = $(METASOURCES)

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc
