all:
	@echo "Run 'make install'."

install:
	/usr/local/bin/ggz-config --install --force --modfile=module.dsc
	cp goclient /usr/local/bin/
	cp goserver.dsc /usr/local/etc/ggzd/games/
	cp goserver.room /usr/local/etc/ggzd/rooms/
	cp goserver /usr/local/bin/

