kioggzmetadocdir = $(kde_htmldir)/en/kioslave
kioggzmetadoc_DATA = ggzmeta.docbook ggzmeta.html

EXTRA_DIST = ggzmeta.docbook

ggzmeta.html: $(top_srcdir)/kio_ggzmeta/kioslave/index.docbook
	$(MEINPROC) --stdout $< > $@

clean-local:
	rm -f $(top_builddir)/kio_ggzmeta/kioslave/ggzmeta.html

