#all:
#	docbook-2-html design.sgml

all: design-html

design-html:
	@SGML2HTML@ design.sgml

clean:
	rm -rf design-html

install:
	$(mkinstalldirs) $(DESTDIR)$(GGZDOCDIR)
	cp -r design-html $(DESTDIR)$(GGZDOCDIR)

