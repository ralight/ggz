if COND_GUIDES
allguides = ggz-game-development-guide.ps ggz-game-development-guide.html ggz-game-development-guide.txt
sourceguide = ggz-game-development-guide.texi
endif

info_TEXINFOS = $(sourceguide)

guidedir = $(GGZDOCDIR)/guides/game-development
guide_DATA = $(allguides) ggz-game-development-guide.sxw

EXTRA_DIST = ggz-game-development-guide.sxw

%.html: %.texi
	texi2html $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f $(allguides)

