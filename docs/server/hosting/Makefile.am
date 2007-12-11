if COND_GUIDES
allguides = ggz-hosting-guide.ps ggz-hosting-guide.html ggz-hosting-guide.txt
sourceguide = ggz-hosting-guide.texi
endif

info_TEXINFOS = $(sourceguide)

guidedir = $(GGZDOCDIR)/guides/hosting
guide_DATA = $(allguides)

%.html: %.texi
	texi2html $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f $(allguides)

