#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libgame.h"

int sdl_init ()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0)
	{
		fprintf (stderr, "SDL init failure: %s\n", SDL_GetError());
		exit(23);
	}
	atexit(SDL_Quit);
	return 0;
}

SDL_Surface *sdl_createwin (int width, int height, int depth, char *title)
{
	SDL_Surface *screen;
	
	screen = SDL_SetVideoMode (width, height, depth, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL)
	{
		fprintf (stderr, "Video error: %s\n", SDL_GetError());
		exit(23);
	}
	SDL_WM_SetCaption (title, NULL);
	return screen;
}

SDL_Surface *sdl_surface (SDL_Surface *screen, int width, int height, int depth)
{
	SDL_Surface *temp;
	Uint32 flags  = screen->flags;
	Uint32 param1 = screen->format->Rmask;
	Uint32 param2 = screen->format->Gmask;
	Uint32 param3 = screen->format->Bmask;
	Uint32 param4 = screen->format->Amask;

	temp = SDL_CreateRGBSurface (flags, width, height, depth, param1, param2, param3, param4);
	if (temp == NULL)
	{
		fprintf (stderr, "SDL Error: %s\n", SDL_GetError());
		exit(23);
	}
	
	return temp;
}

int sdl_drawimg (char *file, SDL_Surface *dest, int x, int y)
{
	SDL_Surface *img;
	SDL_Rect rect, src; 
	
	img = IMG_Load (file);
	if (img == NULL)
	{
		fprintf(stderr, "Loading of the file %s failed.\n", file);
		exit(23);
	}
	
	if ( img->format->palette != NULL )
	{
		SDL_SetColors(dest, img->format->palette->colors, 0, img->format->palette->ncolors);
	}
	
	rect.x = x;
	rect.y = y;
	rect.w = img->w;
	rect.h = img->h;
	src.x  = 0;
	src.y  = 0;
	src.w  = img->w;
	src.h  = img->h;
	
	SDL_BlitSurface (img, &src, dest, &rect);
	SDL_UpdateRects(dest, 1, &rect);
	SDL_FreeSurface(img);
	return 0;
}

int sdl_introimg (char *file, SDL_Surface *dest)
{
	SDL_Surface *img, *black;
	SDL_Rect rect, src, white;

	black = sdl_surface (dest, dest->w, dest->h, 16);
	if (black == NULL)
	{
		fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
		exit(23);
	}
	white.x = 0;
	white.y = 0;
	white.w = dest->w;
	white.h = dest->h;

	
	img = IMG_Load (file);
	if (img == NULL)
	{
		fprintf(stderr, "Loading of the file %s failed.\n", file);
		exit(23);
	}

	if ( img->format->palette != NULL )
	{
		SDL_SetColors(dest, img->format->palette->colors, 0, img->format->palette->ncolors);
        }

	rect.x = (dest->w - img->w)/2;
	rect.y = (dest->h - img->h)/2;
	rect.w = img->w;
	rect.h = img->h;
	
	src.x  = 0;
	src.y  = 0;
	src.w  = img->w;
	src.h  = img->h;

	SDL_BlitSurface (img, &src, dest, &rect);
	SDL_UpdateRects(dest, 1, &rect);	
	SDL_FreeSurface(img);
	SDL_Delay (2000);
	SDL_BlitSurface (black, &white, dest, &white);
	SDL_UpdateRects(dest, 1, &white);
	SDL_FreeSurface(black);
	return 0;
}

int sdl_textout (SDL_Surface *dest, char *text, int x, int y)
{	
	text2gif (text, "/tmp/libgame.gif");
	sdl_drawimg ("/tmp/libgame.gif", dest, x, y);
	unlink ("/tmp/libgame.gif");
	return 0;
}

int sdl_blit (SDL_Surface *src, SDL_Surface *dest)
{
	SDL_Rect temp;

	if (src->w != dest->w || src->h != dest->h)
	{
		fprintf (stderr, "Surfaces are not of the same size!\n");
		exit(23);
	}
	temp.x = 0;
	temp.y = 0;
	temp.w = src->w;
	temp.h = src->h;

	SDL_BlitSurface (src, &temp, dest, &temp);
	SDL_UpdateRects (dest, 1, &temp);
	return 0;
}

int sdl_blitxy (SDL_Surface *src, SDL_Surface *dest, int x, int y)
{
	SDL_Rect rsrc, rdest;

	rsrc.x = 0;
	rsrc.y = 0;
	rsrc.w = src->w;
	rsrc.h = src->h;
	rdest.x = x;
	rdest.y = y;
	rdest.w = rsrc.w;
	rdest.h = rsrc.h;

	SDL_BlitSurface (src, &rsrc, dest, &rdest);
	SDL_UpdateRects (dest, 1, &rdest);
	return 0;
}
