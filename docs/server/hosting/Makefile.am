SUBDIRS = .

info_TEXINFOS = ggz-hosting-guide.texi

guidedir = $(GGZDOCDIR)/guides
#guide_DATA = ggz-hosting-guide.ps
guide_DATA = $(ggzhostingguide)

%_toc.html: %.texi
	texi2html -split_node $<

#%.html: %.texi
#	texi2html -nomenu $<

#%.txt: %.html
#	html2text -o $@ $<

%.txt: %.texi
	$(top_srcdir)/texi2text.pl $< > $@

clean-local:
	-rm -f *.html
	-rm -f *.txt
	-rm -f *.info

install-data-local:
	-cp *.html $(guidedir)

