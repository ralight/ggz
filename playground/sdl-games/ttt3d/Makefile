all:
	gcc -Wall -O2 -o ttt3d main.c display.c -lGL -lSDL -lggzmod

install:
	cp ttt3d /usr/local/lib/ggz
	mkdir -p /usr/local/share/ggz/ttt3d
	cp *.bmp /usr/local/share/ggz/ttt3d/
	/usr/local/bin/ggz-config --install --modfile=module.dsc --force

