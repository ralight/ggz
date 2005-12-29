SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide) ggz-game-development-guide.sxw

EXTRA_DIST = ggz-game-development-guide.sxw

%_toc.html: %.texi
	texi2html -split_node $<
	-mv ggz-game-development-guide/*.html $(top_builddir)/games/game-development/

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f ggz-*.html
	-rm -f $(ggzgamedevelopmentguide)
	-rm -f ggz-game-development-guide.info
	-rm -rf ggz-game-development-guide

install-data-local:
	-cp ggz-*.html $(guidedir)

