SUBDIRS = .

info_TEXINFOS = ggz-hosting-guide.texi

guidedir = $(GGZDOCDIR)/guides
#guide_DATA = ggz-hosting-guide.ps
guide_DATA = $(ggzhostingguide)

%_toc.html: %.texi
	texi2html -split_node $<

clean-local:
	-rm -f *.html

install-data-local:
	-cp *.html $(guidedir)

