SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide) ggz-game-development-guide.sxw

docdir = $(GGZDOCDIR)/games
dist_doc_DATA = games.html features.html

%_toc.html: %.texi
	texi2html -split_node $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html
	-rm -f $(ggzgamedevelopmentguide)
	-rm -f ggz-game-development-guide.info

install-data-local:
	-cp ggz-*.html $(guidedir)

