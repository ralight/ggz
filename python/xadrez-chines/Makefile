all:
	@echo "Run 'make install'."

install:
	mkdir -p /usr/local/share/ggz/xadrez-chines/
	cp *.png /usr/local/share/ggz/xadrez-chines/
	/usr/local/bin/ggz-config --install --force --modfile=module.dsc
	cp xadrez-chines /usr/local/bin/
	cp xadrez-chines-server.dsc /usr/local/etc/ggzd/games/
	cp xadrez-chines-server.room /usr/local/etc/ggzd/rooms/
	cp xadrez-chines-server /usr/local/bin/
	cp xadrezlib.py /usr/local/lib/python2.3/

