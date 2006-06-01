bin_PROGRAMS = telggz

telggz_SOURCES = \
	chat.c chat.h \
	main.c \
	net.c net.h

telggz_LDFLAGS = $(GGZCORE_LDFLAGS)
telggz_LDADD = \
	$(LDADD) \
	$(top_builddir)/metaserv/libmeta/libggzmeta.a \
	$(LIB_GGZCORE)
telggz_CPPFLAGS = \
	-I $(top_srcdir)/metaserv/libmeta \
	$(GGZCORE_INCLUDES)
