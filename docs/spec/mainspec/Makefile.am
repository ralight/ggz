SGML_FILES = mainspec.sgml appendix.sgml client-server.sgml

EXTRA_DIST = $(SGML_FILES) design-txt.dsl

all-local: mainspec-html mainspec.pdf mainspec.txt

mainspec-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/mainspec.sgml
	-mkdir mainspec-html
	-mv *.html mainspec-html
	-cp /usr/share/gtk-doc/data/*.png mainspec-html

mainspec.pdf: $(SGML_FILES)
	-$(SGML2PDF) $(srcdir)/mainspec.sgml

mainspec.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx mainspec.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > mainspec.txt
	-rm tmp.html

clean-local:
	rm -rf mainspec-html
	rm -f mainspec.txt tmp.html
	rm -f mainspec.aux mainspec.jtex mainspec.log mainspec.out mainspec.pdf

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r mainspec-html $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r mainspec.txt $(DESTDIR)$(GGZDOCDIR)/spec
	cp -r mainspec.pdf $(DESTDIR)$(GGZDOCDIR)/spec