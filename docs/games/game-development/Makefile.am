SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide)

docdir = $(GGZDOCDIR)/games
doc_DATA = games.html

%_toc.html: %.texi
	texi2html -split_node $<

clean-local:
	-rm -f ggz-*.html

install-data-local:
	-cp ggz-*.html $(guidedir)
