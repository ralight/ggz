if COND_GUIDES
allguides = ggz-hosting-guide.ps ggz-hosting-guide.html ggz-hosting-guide.txt
endif
sourceguide = ggz-hosting-guide.texi

info_TEXINFOS = $(sourceguide)

guidedir = $(GGZDOCDIR)/guides/hosting
guide_DATA = $(allguides)

%.html: %.texi
	texi2html $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f $(allguides)

