EXTRA_DIST = alien.png franzl.png bg.png geek.png title.png mindx.png ok.png

install-data-local:
	$(mkinstalldirs) $(ggz_datadir)/krosswater/gfx
	$(INSTALL_DATA) *.png $(ggz_datadir)/krosswater/gfx/

uninstall-local:
	-rm -f $(ggz_datadir)/krosswater/gfx/*.png
	-rm -f $(ggz_datadir)/krosswater/gfx
