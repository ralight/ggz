#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifndef NOGTK
#include <gtk/gtk.h>
#endif
#include <libgame.h>

//Some stuff
#ifdef RELEASE
#undef VERBOSE        
#undef NOINTRO        
#undef NOMENU          
#undef PRINTMOUSE      
#undef CHEAT_DEBUG     
#undef NOGOSCREEN
#endif

#define BGDIR           stralloc (PREFIX, "/pixmaps/bg/")
#define BTNDIR          stralloc (PREFIX, "/pixmaps/bg/buttons/")
#define INTRODIR        stralloc (PREFIX, "/pixmaps/intro/")
#define SPRITEDIR       stralloc (PREFIX, "/pixmaps/sprites/")
#define WAVDIR          stralloc (PREFIX, "/sounds/")
#define TEXTDIR         stralloc (PREFIX, "/docs/")
#define EXPDIR          stralloc (PREFIX, "/pixmaps/explosions/")
#define DEXPDIR         stralloc (PREFIX, "/pixmaps/explosions/deathstar/")
#define HIGHFILE        stralloc (PREFIX, "/highscore/hai.dat")

// Global defines
#define TITLE	"Star Wars: Episode 23"
#define VERSION	" version 0.55a"

#ifndef TRUE
#define TRUE 	1
#define FALSE	0
#endif

#define ENEMY		2

#define INIT		0
#define INTRO		1
#define MENU		2
#define GAME		3
#define SHUTDOWN	4
#define CREDITS		5
#define	GAME_OVER	6
#define PAUSE		7
#define HIGHSCORE	8

// Structure definitions
typedef struct
{
	char *file;
	int visible;
	int speed;
	int movex, movey;
	int x, y;
	int w, h;
} Ship;

typedef struct
{
	SDL_Surface **bmp;
	SDL_Surface *mov;
	SDL_Surface *std;
	int x, y;
	int w, h;
} Button;

typedef struct
{
	char name[100];
	int score;
} HaiScore;

// Global functions
void set_ship (int type);
void show_text (char *filename);
void optionscreen ();

// Global variables
int show_credits, outride, invul, littlestar;
int pdef;
SDL_Surface *window;
Ship player;
HaiScore scorelist[10];
