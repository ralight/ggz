#include <SDL.h>
#include <SDL_image.h>
#include <esd.h>
#include <gif_lib.h>
#include <pthread.h>

// General functions
char *stralloc (char *string1, char *string2);
void text2gif (char *text, char *filename);

// SDL video functions
int sdl_init ();
SDL_Surface *sdl_createwin (int width, int height, int depth, char *title);
SDL_Surface *sdl_surface (SDL_Surface *screen, int width, int height, 
		int depth);
int sdl_drawimg (char *file, SDL_Surface *dest, int x, int y);
int sdl_introimg (char *file, SDL_Surface *dest);
int sdl_textout (SDL_Surface *dest, char *text, int x, int y);
int sdl_blit (SDL_Surface *src, SDL_Surface *dest);
int sdl_blitxy (SDL_Surface *src, SDL_Surface *dest, int x, int y);

// ESD audio functions
void esd_playwav (char *filename);
