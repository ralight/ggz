SUBDIRS = .

#KDE_LANG = en
#KDE_DOCS = AUTO

kioggzmetadocdir = $(kde_htmldir)/en/kioslave
kioggzmetadoc_DATA = ggzmeta.docbook ggzmeta.html

ggzmeta.html: $(top_srcdir)/kio_ggzmeta/kioslave/index.docbook
	$(MEINPROC) --stdout $< > $@

#install-data-local:
#	-@echo "IOSlave DocBook hack..."
#	-$(top_srcdir)/kio_ggz/kioslave/ioslave.pl $(kde_htmldir) ggz

clean-local:
	rm -f $(top_builddir)/kio_ggzmeta/kioslave/ggzmeta.html

#install-data-local:
#	$(mkinstalldirs) $(kde_htmldir)/en/kioslave
#	$(INSTALL_DATA) $(top_builddir)/kio_ggz/kioslave/ggz.html $(kde_htmldir)/en/kioslave

