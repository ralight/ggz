EXTRA_DIST = bishop_b.xpm knight_b.xpm queen_b.xpm rook_b.xpm king_b.xpm \
			 bishop_w.xpm knight_w.xpm queen_w.xpm rook_w.xpm king_w.xpm

SUBDIRS = .

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/koenig/pics
	$(INSTALL) -m 644 *.xpm $(DESTDIR)$(ggzdatadir)/koenig/pics/

uninstall-local:
	-rm -rv $(ggzdatadir)/koenig

