info_TEXINFOS = ggz-hosting-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzhostingguide)

%_toc.html: %.texi
	texi2html -split_node $<
	-mv ggz-hosting-guide/*.html $(top_builddir)/server/hosting/

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html
	-rm -f $(ggzhostingguide)
	-rm -f ggz-hosting-guide.info
	-rm -rf ggz-hosting-guide

install-data-local:
	-cp ggz-*.html $(guidedir)

