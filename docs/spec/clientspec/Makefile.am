SUBDIRS = .

SGML_FILES = clientspec.sgml

EXTRA_DIST = $(SGML_FILES) design-txt.dsl

all: clientspec-html clientspec.pdf clientspec.txt

clientspec-html: $(SGML_FILES)
	$(SGML2HTML) $(srcdir)/clientspec.sgml
	-mkdir clientspec-html
	-cp /usr/share/gtk-doc/data/*.png clientspec-html
	-mv *.html clientspec-html

clientspec.pdf: $(SGML_FILES)
	$(SGML2PDF) $(srcdir)/clientspec.sgml

clientspec.txt: design-txt.dsl $(SGML_FILES)
	$(JADE) -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx clientspec.sgml > tmp.html
	$(LYNX) -dump tmp.html -nolist > clientspec.txt
	-rm tmp.html

clean:
	rm -rf clientspec-html
	rm -f clientspec.txt tmp.html
	rm -f clientspec.aux clientspec.jtex clientspec.log clientspec.out clientspec.pdf

install:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)/spec/clientspec
	cp -r clientspec-html $(DESTDIR)$(GGZDOCDIR)/spec/clientspec
	cp -r clientspec.txt $(DESTDIR)$(GGZDOCDIR)/spec/clientspec
	cp -r clientspec.pdf $(DESTDIR)$(GGZDOCDIR)/spec/clientspec
