SUBDIRS = .

bin_PROGRAMS = grubby

grubby_SOURCES = agrub.c \
		 chat.c		chat.h \
		 commands.c	commands.h \
		 popt.c 	popt.h \
		 init.c 	init.h \
		 log.c		log.h \
		 loop.c 	loop.h \
		 net.c		net.h \
		 rc.c		rc.h \
		 support.c	support.h 

grubby_LDADD = $(LDADD) $(grubby_LIBS)

