SUBDIRS = . html

EXTRA_DIST = README.ggzcore++ kdoc-generated.html

install-data-local:
	$(mkinstalldirs) $(prefix)/share/doc/ggzcore++
	$(INSTALL_DATA) README.ggzcore++ $(prefix)/share/doc/ggzcore++/
	$(INSTALL_DATA) kdoc-generated.html $(prefix)/share/doc/ggzcore++/

uninstall-local:
	-rm -f $(prefix)/share/doc/ggzcore++
