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
	install -m 644 ggz.docbook $(kde_htmldir)/en/kioslave
	install -m 644 ggz.html $(kde_htmldir)/en/kioslave
