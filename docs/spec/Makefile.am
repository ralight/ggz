#all:
#	docbook-2-html design.sgml

SGML_FILES = appendix.sgml design.sgml overview.sgml client-server.sgml \
		modules.sgml requirements.sgml

EXTRA_DIST = $(SGML_FILES) design.dsl design-txt.dsl

all: design-html design.txt

design-html: $(SGML_FILES)
	@SGML2HTML@ $(srcdir)/design.sgml
	-mkdir design-html
	-mv *.html design-html

design.txt: design-txt.dsl $(SGML_FILES)
	@JADE@ -tsgml -ilynx -d$(srcdir)/design-txt.dsl\#lynx design.sgml > tmp.html
	@LYNX@ -dump tmp.html -nolist > design.txt
	-rm tmp.html

clean:
	rm -rf design-html design.txt tmp.html

install:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)
	cp -r design-html $(DESTDIR)$(GGZDOCDIR)
	cp -r design.txt $(DESTDIR)$(GGZDOCDIR)
