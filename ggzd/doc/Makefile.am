
sgml_dir = /net/rimshot2/local/share

SUBDIRS = .

SGML_FILES = design.sgml
TXT_FILES = design.txt
HTML_FILES = design.html
PS_FILES = design.ps

FIG_FILES  = images/server_arch.fig

EXTRA_DIST = $(SGML_FILES) \
	     $(FIG_FILES)

all-docs: html-docs txt-docs

txt-docs: txtdir $(TXT_FILES) 

html-docs: htmldir gif-files $(HTML_FILES) 
	-cp images/*.gif html

ps-docs: psdir eps-files $(PS_FILES) 

htmldir:
	(cd $(srcdir); mkdir -p html;)

txtdir:
	(cd $(srcdir); mkdir -p txt;)

psdir:
	(cd $(srcdir); mkdir -p ps;)
	
%.txt: %.sgml
	-cd $(srcdir) && jade -t sgml -dascii-lynx.dsl $^ > tmp.html
	-lynx -dump tmp.html -nolist > txt/$@
	-rm tmp.html

%.html: %.sgml
	-cd $(srcdir) && db2html $^
	-mv $* html

%.ps: %.sgml
	-cd $(srcdir) && db2ps $^
	-mv *.ps ps
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
	mkdir -p $(distdir)/html
	-cp -a $(srcdir)/html/* $(distdir)/html
	-cp $(srcdir)/txt $(distdir)
	mkdir -p $(distdir)/images
	-cp $(srcdir)/images/*.gif $(srcdir)/images/*.jpg $(distdir)/images

#install-data-local: 
#	$(mkinstalldirs) $(DESTDIR)$(gnumeric_helpdir)/images
#	-for file in $(srcdir)/gnumeric/*.html $(srcdir)/gnumeric/*.css; do \
#	basefile=`basename $$file`; \
#	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/$$basefile; \
#	done
#	-for file in $(srcdir)/images/*.jpg $(srcdir)/images/*.gif; do \
#	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/images;\
#	done

