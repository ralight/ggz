## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4646 2002-09-21 10:11:00Z dr_maux $

SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide)

docdir = $(GGZDOCDIR)/games
doc_DATA = games.html features.html

EXTRA_DIST = $(doc_DATA)

%_toc.html: %.texi
	texi2html -split_node $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html

install-data-local:
	-cp ggz-*.html $(guidedir)
