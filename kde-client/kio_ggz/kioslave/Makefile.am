kioggzdocdir = $(kde_htmldir)/en/kioslave
kioggzdoc_DATA = ggz.docbook ggz.html

EXTRA_DIST = ggz.docbook

ggz.html: $(top_srcdir)/kio_ggz/kioslave/index.docbook
	$(MEINPROC) --stdout $< > $@

clean-local:
	rm -f $(top_builddir)/kio_ggz/kioslave/ggz.html

