prefix = /usr/local
version = 0.0.10

all:
	@echo Building library PyGGZMod
	cd pyggzmod && python setup.py build
	@echo Building library PyGGZdMod
	cd pyggzdmod && python setup.py build
	@echo Building library PyGGZCoreSimple
	cd pyggzcore && python setup.py build

install:
	@echo Installing library PyGGZMod
	cd pyggzmod && python setup.py install --prefix=$(DESTDIR)$(prefix)
	@echo Installing library PyGGZdMod
	cd pyggzdmod && python setup.py install --prefix=$(DESTDIR)$(prefix)
	@echo Installing library PyGGZCoreSimple
	cd pyggzcore && python setup.py install --prefix=$(DESTDIR)$(prefix)

	@echo Installing game Dresden
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp dresden/dresden.py $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/dresden/
	cp dresden/*.png $(DESTDIR)$(prefix)/share/ggz/dresden

	@echo Installing game Escape/SDL
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp escape-sdl/escape-sdl $(DESTDIR)$(prefix)/bin
	ggz-config --install --force --modfile=escape-sdl/module.dsc

	@echo Installing game Xadrez Chines
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp xadrez-chines/xadrez-chines $(DESTDIR)$(prefix)/bin
	cp xadrez-chines/xadrez-chines-server $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/lib/site-python/
	cp xadrez-chines/xadrezlib.py $(DESTDIR)$(prefix)/lib/site-python
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/xadrez-chines/
	cp xadrez-chines/*.png $(DESTDIR)$(prefix)/share/ggz/xadrez-chines
	ggz-config --install --force --modfile=xadrez-chines/module.dsc

	@echo Installing external game GNU Go support files
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp go/goclient $(DESTDIR)$(prefix)/bin
	cp go/goserver $(DESTDIR)$(prefix)/bin
	cp go/goclient.helper $(DESTDIR)$(prefix)/bin
	ggz-config --install --force --modfile=go/module.dsc

	@echo Installing core client Vibora
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp vibora/vibora $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/vibora/
	cp vibora/*.png $(DESTDIR)$(prefix)/share/ggz/vibora

	@echo Installing SDL newstuff dialog
	mkdir -p $(DESTDIR)$(prefix)/lib/site-python/
	cp lib/sdlnewstuff.py $(DESTDIR)$(prefix)/lib/site-python

dist:
	rm -rf ../ggz-python-$(version)
	cp -r ../python ../ggz-python-$(version)
	find ../ggz-python-$(version) -name CVS | xargs rm -rf
	cd .. && tar cvzf ggz-python-$(version).tar.gz ggz-python-$(version)

