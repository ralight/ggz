SUBDIRS = serverspec clientspec mainspec

SGML_FILES = design.sgml overview.sgml requirements.sgml

EXTRA_DIST = $(SGML_FILES) design.dsl design-txt.dsl server_arch.fig Uri-spec.txt

all-local: design-html design.pdf design.txt

design-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/design.sgml
	mkdir -p design-html
	-mv *.html design-html
	-cp design-html/book1.htm design-html/index.html
	$(FIG2DEV) -Ljpeg $(srcdir)/server_arch.fig > design-html/server_arch.jpg

design.pdf: $(SGML_FILES)
	$(FIG2DEV) -Ljpeg $(srcdir)/server_arch.fig > server_arch.jpg
	-$(SGML2PDF) $(srcdir)/design.sgml

design.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx $(srcdir)/design.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > design.txt
	-rm tmp.html

clean-local:
	rm -rf design-html
	rm -f design.txt tmp.html
	rm -f design.aux design.jtex design.log design.out design.pdf
	rm -f jadetex.cfg server_arch.jpg

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/design
	cp -r design-html $(DESTDIR)$(GGZDOCDIR)/design
	cp -r design.txt $(DESTDIR)$(GGZDOCDIR)/design
	cp -r design.pdf $(DESTDIR)$(GGZDOCDIR)/design
