version = 0.0.14pre

CONFIGURATION = .ggzcommunity.setup

all: compile
	#echo "Please read setup/setup.txt."

setup:
	cd setup && ./setup.sh $(CONFIGURATION)

compile:
	cd setup && ./setup-build.sh $(CONFIGURATION)

install:
	cd setup && ./setup-install.sh $(CONFIGURATION)

debcompile:
	cd setup/scripts/calcrankings && make

debinstall:
	mkdir -p $(DESTDIR)/var/www
	cp -r web $(DESTDIR)/var/www/ggzcommunity
	mkdir -p $(DESTDIR)/etc/apache2/sites-available
	# FIXME: ship template and install afterwards
	#cp setup/webserver/ggz-apache2.conf $(DESTDIR)/etc/apache2/sites-available/ggzcommunity
	cd setup/scripts/calcrankings && make install
	cp setup/scripts/ggz2phpbb.pl $(DESTDIR)/usr/bin/ggz2phpbb
	cp setup/scripts/ggzblogs.pl $(DESTDIR)/usr/bin/ggzblogs

clean:
	rm -f setup/scripts/calcrankings/calcrankings

dist:
	rm -rf ../ggz-community-$(version)
	cp -r ../community ../ggz-community-$(version)
	cd ../ggz-community-$(version) && svn-clean
	find ../ggz-community-$(version) -name .svn | xargs rm -rf
	cd .. && tar cvzf ggz-community-$(version).tar.gz ggz-community-$(version)

.PHONY: setup
