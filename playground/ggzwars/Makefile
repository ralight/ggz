
CC=gcc
DEBUG=-g3 -Wall
RELEASE=-DRELEASE
CFLAGS=-Ilibgame `sdl-config --cflags` `gtk-config --cflags`
LDFLAGS=-Llibgame -lgame -lgif -lSDL_image `sdl-config --libs` `gtk-config --libs`
#CFLAGS+=-DVERBOSE        
#CFLAGS+=-DNOINTRO         
#CFLAGS+=-DNOMENU          
#CFLAGS+=-DPRINTMOUSE      
#CFLAGS+=-DCHEAT_DEBUG     
#CFLAGS+=-DNOSOUND         
#CFLAGS+=-DNOGTK           
#CFLAGS+=-DNOGOSCREEN
BINARY=StarWars
FILES=src/interface.c src/ships.c src/main.c
BIN_PREFIX=/usr/games
DATA_PREFIX=/usr/share/games/starwars

# !!! Do not edit this !!!
all: 
	$(CC) $(DEBUG) $(CFLAGS) -o $(BINARY) $(FILES) $(LDFLAGS)
release: 
	$(CC) $(RELEASE) $(CFLAGS) -o $(BINARY) $(FILES) $(LDFLAGS)
clean:
	rm -f *.o $(BINARY)
install:
	mkdir $(DATA_PREFIX)
	cp -R data/docs $(DATA_PREFIX)
	cp -R data/highscore $(DATA_PREFIX) 
	cp -R data/pixmaps $(DATA_PREFIX) 
	cp -R data/sounds $(DATA_PREFIX)
	cp $(BINARY) $(BIN_PREFIX)
uninstall:
	rm -f $(BIN_PREFIX)/$(BINARY)
	rm -fr $(DATA_PREFIX)
