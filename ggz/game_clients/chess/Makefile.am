## Process this file with automake to produce Makefile.in

EXTRA_DIST = chess.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@

ggzexecmod_PROGRAMS = ggz.chess

ggz_chess_SOURCES = main_win.c \
                    main_win.h \
                    support.c \
                    support.h \
                    callbacks.c \
                    callbacks.h \
                    main.c

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_chess_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@
