
sgml_dir = /net/rimshot2/local/share

SUBDIRS = .

SGML_FILES = design.sgml
TXT_FILES = txt/design.txt 
HTML_FILES = html/design/index.html
PS_FILES = ps/design.ps

FIG_FILES  = images/server_arch.fig

EXTRA_DIST = $(SGML_FILES) \
	     $(FIG_FILES) \
	     game-modules.txt


all-docs: html-docs txt-docs

txt-docs: txtdir $(TXT_FILES) 

html-docs: htmldir $(HTML_FILES)

ps-docs: psdir $(PS_FILES) eps-files


# Directories to hold the docs
htmldir:
	(cd $(srcdir); mkdir -p html;)

txtdir:
	(cd $(srcdir); mkdir -p txt;)

psdir:
	(cd $(srcdir); mkdir -p ps;)


# Files for the design-doc
html/design/index.html: design.sgml gif-files
	-cd $(srcdir) && db2html -d ../ggz.dsl design.sgml
	-mv design html
	-mv images/server_arch.gif html/design

txt/design.txt: design.sgml
	-cd $(srcdir) && jade -tsgml -ilynx -dggz.dsl\#lynx design.sgml > tmp.html
	-lynx -dump tmp.html -nolist > txt/design.txt
	-rm tmp.html

ps/design.ps: design.sgml
	-cd $(srcdir) && db2ps design.sgml
	-mv design.ps ps
	-rm -f *.tex *.aux *.dvi *.log


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


dist-hook: html-docs txt-docs
	mkdir -p $(distdir)/html
	-cp -a $(srcdir)/html/* $(distdir)/html
	-cp $(srcdir)/txt/* $(distdir)
	mkdir -p $(distdir)/images
	-cp $(srcdir)/images/*.gif $(distdir)/images

#install-data-local: 
#	$(mkinstalldirs) $(DESTDIR)$(gnumeric_helpdir)/images
#	-for file in $(srcdir)/gnumeric/*.html $(srcdir)/gnumeric/*.css; do \
#	basefile=`basename $$file`; \
#	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/$$basefile; \
#	done
#	-for file in $(srcdir)/images/*.jpg $(srcdir)/images/*.gif; do \
#	$(INSTALL_DATA) $$file $(DESTDIR)$(gnumeric_helpdir)/images;\
#	done

