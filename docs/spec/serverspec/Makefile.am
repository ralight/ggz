SUBDIRS = .

SGML_FILES = serverspec.sgml

EXTRA_DIST = $(SGML_FILES) design-txt.dsl

all: serverspec-html serverspec.pdf serverspec.txt

serverspec-html: $(SGML_FILES)
	$(SGML2HTML) --style local=design $(srcdir)/serverspec.sgml
	-mkdir serverspec-html
	-cp /usr/share/gtk-doc/data/*.png serverspec-html
	-mv *.html serverspec-html

serverspec.pdf: $(SGML_FILES)
	$(SGML2PDF) $(srcdir)/serverspec.sgml

serverspec.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx serverspec.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > serverspec.txt
	-rm tmp.html

clean:
	rm -rf serverspec-html
	rm -f serverspec.txt tmp.html
	rm -f serverspec.aux serverspec.jtex serverspec.log serverspec.out serverspec.pdf

install:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec-html $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec.txt $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
	cp -r serverspec.pdf $(DESTDIR)$(GGZDOCDIR)/spec/serverspec
