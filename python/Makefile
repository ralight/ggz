prefix = /usr/local

all:
	@echo Building library PyGGZMod
	cd pyggzmod && python setup.py build
	@echo Building library PyGGZdMod
	cd pyggzdmod && python setup.py build

install:
	@echo Installing library PyGGZMod
	cd pyggzmod && python setup.py install
	@echo Installing library PyGGZdMod
	cd pyggzdmod && python setup.py install

	@echo Installing game Dresden
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp dresden/dresden.py $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/dresden/
	cp dresden/*.png $(DESTDIR)$(prefix)/share/ggz/dresden
	@echo Installing game Escape/SDL
	mkdir -p $(DESTDIR)$(prefix)/bin/
	cp escape-sdl/escape-sdl $(DESTDIR)$(prefix)/bin
	ggz-config --install --force --modfile=escape-sdl/module.dsc

