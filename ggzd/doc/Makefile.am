SUBDIRS =

SGML_FILES = design.sgml

FIG_FILES  = images/server_arch.fig

EXTRA_DIST = $(SGML_FILES) \
	     $(FIG_FILES)

all-docs: html-docs, txt-docs, ps-docs

txt-docs: $(SGML_FILES) 
	-mkdir txt
	-cd $(srcdir) && jade -t sgml -d /usr/lib/sgml/stylesheet/dsssl/sgmltools/ascii-lynx.dsl design.sgml > design.html
	-lynx -dump design.html -nolist > txt/ggz-design.txt
	-rm design.html

html-docs: $(SGML_FILES) gif-files
	-cd $(srcdir) && db2html design.sgml
	-rm -rf html
	-mv design html
	-cp images/*.gif html

ps-docs: $(SGML_FILES) eps-files
	-mkdir ps
	-for file in $(SGML_FILES); do \
	db2ps $$file; \
	done
	-rm -f *.tex *.aux *.dvi *.log *.html

gif-files: $(FIG_FILES)
	-for file in $(FIG_FILES); do \
	basefile=`basename $$file .fig`; \
	fig2dev -Lgif $$file images/$$basefile.gif; \
	done

eps-files: $(FIG_FILES)
	-for file in $(FIG_FILES); do \
	basefile=`basename $$file .fig`; \
	fig2dev -Leps $$file images/$$basefile.eps; \
	done

dist-hook: html-docs
	mkdir $(distdir)/html
	-cp $(srcdir)/html/* $(distdir)/gnumeric
	-cp $(srcdir)/txt $(distdir)
#	mkdir $(distdir)/images
#	-cp $(srcdir)/images/*.gif $(srcdir)/images/*.jpg $(distdir)/images

install-data-local: 
	$(mkinstalldirs) $(DESTDIR)$(gnumeric_helpdir)/images
	-for file in $(srcdir)/gnumeric/*.html $(srcdir)/gnumeric/*.css; do \
	basefile=`basename $$file`; \
	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/$$basefile; \
#	done
#	-for file in $(srcdir)/images/*.jpg $(srcdir)/images/*.gif; do \
#	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/images;\
#	done

