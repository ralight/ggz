SUBDIRS = .

#KDE_LANG = en
#KDE_DOCS = AUTO

all: ggz.html

ggz.html:
	$(MEINPROC) --stdout index.docbook > ggz.html

#install-data-local:
#	-@echo "IOSlave DocBook hack..."
#	-$(top_srcdir)/kio_ggz/kioslave/ioslave.pl $(kde_htmldir) ggz

install-data-local:
	$(mkinstalldirs) $(kde_htmldir)/en/kioslave
	$(INSTALL_DATA) ggz.docbook $(kde_htmldir)/en/kioslave
	$(INSTALL_DATA) ggz.html $(kde_htmldir)/en/kioslave
