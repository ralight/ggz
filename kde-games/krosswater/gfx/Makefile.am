EXTRA_DIST = alien.png franzl.png bg.png geek.png title.png mindx.png ok.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/krosswater/gfx
	$(INSTALL_DATA) *.png $(DESTDIR)$(ggzdatadir)/krosswater/gfx/

uninstall-local:
	-rm -f $(ggzdatadir)/krosswater/gfx/*.png
	-rm -f $(ggzdatadir)/krosswater/gfx
