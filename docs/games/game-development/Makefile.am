SUBDIRS = .

info_TEXINFOS = ggz-game-development-guide.texi

guidedir = $(GGZDOCDIR)/guides
guide_DATA = $(ggzgamedevelopmentguide)

.html: .texi
·   texi2html -split_node $<

clean-local:
·   -rm -f *.html

