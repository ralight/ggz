SGML_FILES = serverspec.sgml

EXTRA_DIST = $(SGML_FILES) design-txt.dsl

all-local: serverspec-html serverspec.pdf serverspec.txt

serverspec-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/serverspec.sgml
	mkdir -p serverspec-html
	#-cp /usr/share/gtk-doc/data/*.png serverspec-html
	-mv *.html serverspec-html
	-cp serverspec-html/book1.htm serverspec-html/index.html

serverspec.pdf: $(SGML_FILES)
	$(SGML2PDF) $(srcdir)/serverspec.sgml

serverspec.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx $(srcdir)/serverspec.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > serverspec.txt
	-rm tmp.html

clean-local:
	rm -rf serverspec-html
	rm -f serverspec.txt tmp.html
	rm -f serverspec.aux serverspec.jtex serverspec.log serverspec.out serverspec.pdf

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec-html $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec.txt $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec.pdf $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
