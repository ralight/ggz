## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4598 2002-09-17 03:15:49Z jdorje $

SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide)

docdir = $(GGZDOCDIR)/games
doc_DATA = games.html

EXTRA_DIST = $(doc_DATA)

%_toc.html: %.texi
	texi2html -split_node $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html

install-data-local:
	-cp ggz-*.html $(guidedir)
