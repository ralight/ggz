SUBDIRS = .

#KDE_LANG = en
#KDE_DOCS = AUTO

kioggzdocdir = $(kde_htmldir)/en/kioslave
kioggzdoc_DATA = ggz.docbook ggz.html

ggz.html: $(top_srcdir)/kio_ggz/kioslave/index.docbook
	$(MEINPROC) --stdout $< > $@

#install-data-local:
#	-@echo "IOSlave DocBook hack..."
#	-$(top_srcdir)/kio_ggz/kioslave/ioslave.pl $(kde_htmldir) ggz

clean-local:
	rm -f $(top_builddir)/kio_ggz/kioslave/ggz.html

#install-data-local:
#	$(mkinstalldirs) $(kde_htmldir)/en/kioslave
#	$(INSTALL_DATA) $(top_builddir)/kio_ggz/kioslave/ggz.html $(kde_htmldir)/en/kioslave

