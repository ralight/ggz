SUBDIRS = serverspec clientspec

SGML_FILES = appendix.sgml design.sgml overview.sgml client-server.sgml \
		modules.sgml requirements.sgml

EXTRA_DIST = $(SGML_FILES) design.dsl design-txt.dsl

all: design-html design.pdf design.txt

design-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/design.sgml
	-mkdir design-html
	-mv *.html design-html
	-cp /usr/share/gtk-doc/data/*.png design-html
	$(FIG2DEV) -Ljpeg server_arch.fig > design-html/server_arch.jpg

design.pdf: $(SGML_FILES)
	$(FIG2DEV) -Ljpeg server_arch.fig > server_arch.jpg
	$(SGML2PDF) $(srcdir)/design.sgml

design.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx design.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > design.txt
	-rm tmp.html

clean:
	rm -rf design-html
	rm -f design.txt tmp.html
	rm -f design.aux design.jtex design.log design.out design.pdf

install:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r design-html $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r design.txt $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r design.pdf $(DESTDIR)$(GGZDOCDIR)/spec
