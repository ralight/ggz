SUBDIRS = .

#KDE_LANG = en
#KDE_DOCS = AUTO

all: ggzmeta.html

ggzmeta.html:
	$(MEINPROC) --stdout index.docbook > ggzmeta.html

#install-data-local:
#	-@echo "IOSlave DocBook hack..."
#	-$(top_srcdir)/kio_ggz/kioslave/ioslave.pl $(kde_htmldir) ggz

install-data-local:
	install -m 644 ggzmeta.docbook $(kde_htmldir)/en/kioslave
	install -m 644 ggzmeta.html $(kde_htmldir)/en/kioslave

