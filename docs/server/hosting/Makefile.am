SUBDIRS = .

info_TEXINFOS = ggz-hosting-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzhostingguide)

%_toc.html: %.texi
	texi2html -split_node $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html
	-rm -f $(ggzhostingguide)
	-rm -f ggz-hosting-guide.info

install-data-local:
	-cp ggz-*.html $(guidedir)

