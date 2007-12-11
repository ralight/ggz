SGML_FILES = mainspec.sgml appendix.sgml client-server.sgml

EXTRA_DIST = $(SGML_FILES) design-txt.dsl

all-local: mainspec-html mainspec.pdf mainspec.txt

mainspec-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/mainspec.sgml
	mkdir -p mainspec-html
	-mv *.html mainspec-html
	-cp mainspec-html/book1.htm mainspec-html/index.html

mainspec.pdf: $(SGML_FILES)
	-$(SGML2PDF) $(srcdir)/mainspec.sgml

mainspec.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx $(srcdir)/mainspec.sgml > tmp.html
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
