/*
 * Geekgame - a game which only real geeks understand
 * Copyright (C) 2002 - 2004 Josef Spillner <josef@ggzgamingzone.org>
 * $Id: intro.c 6759 2005-01-20 05:17:31Z jdorje $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include "intro.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <GL/gl.h>

#include "config.h"

#define texture_none 0
#define texture_board 1
#define texture_lines 2
#define texture_banner 3

#define TEXTURESIZE 1048576
#define TEXTURECOUNT 4
#define PIECES 30

static void draw(void);
static void idle(void);
static void reshape(int width, int height);
static void init(void);

extern SDL_Surface *screen;

static unsigned char texture[TEXTURESIZE][4];
static long texturevalue[TEXTURECOUNT][4];
static int maxtexturecount = 0;
static int maxtexture = 0;

static GLfloat view_rotx = 0.0, view_roty = 30.0;
static GLfloat angle = 50.0;
static GLfloat zoom = 0.5;
static int use_alpha = 0;
static int alpha = 0;

struct piece_t
{
	int value;
	GLfloat x, y, z;
};

static struct piece_t pieces[PIECES];

static void teximage(int tex)
{
	long n = -1;
	long i;

	for(i = 0; i < maxtexturecount; i++) if(texturevalue[i][3] == tex) n = i;
	if(n == -1) return;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturevalue[n][0], texturevalue[n][1],
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture[texturevalue[n][2]]);
}

static void alphaimage(int tex, int alpha)
{
	long n = -1;
	long i;

	for(i = 0; i < maxtexturecount; i++) if(texturevalue[i][3] == tex) n = i;
	if(n == -1) return;
	for(i = 0; i < texturevalue[n][0] * texturevalue[n][1]; i++)
		texture[i][3] = alpha;
}

static void drawbox(GLfloat width, GLfloat height, GLfloat depth, int tex)
{
	height = -height;

	glPushMatrix();
	glTranslatef(-width / 2.0, -height / 2.0, -depth / 2.0);

	/* use textures */
	if(tex != texture_none)
	{
		teximage(tex);
		glEnable(GL_TEXTURE_2D);
	}
	if(use_alpha) glEnable(GL_BLEND);

	/* front plate */
	glNormal3f(0.0, 0.0, 1.0);

	glBegin(GL_QUADS);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(width, height, depth);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(width, 0.0, depth);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, depth);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, height, depth);
	glEnd();

	/* back plate */
	glNormal3f(0.0, 0.0, 1.0);

	glBegin(GL_QUADS);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, height, 0.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(width, 0.0, 0.0);
	glTexCoord3f(1.0, 1.0, 0.0);
	glVertex3f(width, height, 0.0);
	glEnd();

	/* left plate */
	glNormal3f(0.0, 1.0, 0.0);

	glBegin(GL_QUADS);
	glTexCoord3f(0.0, 1.0, 1.0);
	glVertex3f(0.0, height, depth);
	glTexCoord3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, depth);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, height, 0.0);
	glEnd();

	/* right plate */
	glNormal3f(0.0, 1.0, 0.0);

	glBegin(GL_QUADS);
	glTexCoord3f(0.0, 1.0, 0.0);
	glVertex3f(width, height, 0.0);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(width, 0.0, 0.0);
	glTexCoord3f(0.0, 0.0, 1.0);
	glVertex3f(width, 0.0, depth);
	glTexCoord3f(0.0, 1.0, 1.0);
	glVertex3f(width, height, depth);
	glEnd();

	/* board on other side */
	glNormal3f(1.0, 0.0, 0.0);

	glBegin(GL_QUADS);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glTexCoord3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, depth);
	glTexCoord3f(1.0, 0.0, 1.0);
	glVertex3f(width, 0.0, depth);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(width, 0.0, 0.0);
	glEnd();

	/* game board */
	glNormal3f(1.0, 0.0, 0.0);

	glBegin(GL_QUADS);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(width, height, 0.0);
	glTexCoord3f(1.0, 0.0, 1.0);
	glVertex3f(width, height, depth);
	glTexCoord3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, height, depth);
	glTexCoord3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, height, 0.0);
	glEnd();

	if(tex != texture_none) glDisable(GL_TEXTURE_2D);
	if(use_alpha) glDisable(GL_BLEND);

	glPopMatrix();
}

static void drawbanner(void)
{
	static GLfloat green[4] = {0.0, 0.7, 0.0, 1.0};

	/* (1) draw the board */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);

	use_alpha = 1;
	drawbox(12.0, 3.0, 1.0, texture_banner);
	use_alpha = 0;
}

static void drawnumber(int number)
{
	static GLfloat green[4] = {0.0, 0.7, 0.0, 1.0};
	static GLfloat darkgreen[4] = {0.0, 0.4, 0.0, 1.0};

	/* (1) draw the board */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, darkgreen);

	drawbox(10.0, 1.0, 10.0, texture_board);

	/* (2) draw the lines on the board */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);

	if(number == 0)
	{
		glPushMatrix();
		glTranslatef(1.7, 0.7, 0.0);
		drawbox(0.2, 0.4, 6.0, texture_lines);
		glTranslatef(-3.4, 0.0, 0.0);
		drawbox(0.2, 0.4, 6.0, texture_lines);
		glTranslatef(1.7, 0.0, 0.0);

		glTranslatef(0.0, 0.0, 3.0);
		drawbox(3.4, 0.4, 0.2, texture_lines);
		glTranslatef(0.0, -0.0, -6.0);
		drawbox(3.4, 0.4, 0.2, texture_lines);
		glTranslatef(0.0, 0.0, 3.0);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glTranslatef(0.1, 0.7, 0.0);
		drawbox(0.2, 0.4, 6.0, texture_lines);
		glTranslatef(-0.1, 0.0, 0.0);
		glPopMatrix();
	}
}

static void textureloader(char *filename, int width, int height, int tex, int alpha)
{
	long i, j, k;
	FILE *texfile;
	
	texfile = fopen(filename, "r");
	if(texfile)
	{
		texturevalue[maxtexturecount][0] = width;
		texturevalue[maxtexturecount][1] = height;
		texturevalue[maxtexturecount][2] = maxtexture;
		texturevalue[maxtexturecount][3] = tex;
		maxtexturecount++;

		for(i = 0; i < 54; i++) k = getc(texfile);
			for(j = 0; j < height; j++)
			{
				for(i = 0; i < width; i++)
				{
					texture[maxtexture][2] = getc(texfile);
					texture[maxtexture][1] = getc(texfile);
					texture[maxtexture][0] = getc(texfile);
					texture[maxtexture][3] = alpha;
					maxtexture++;
				}
			}
		fclose(texfile);
	}
}

void draw(void)
{
	int i;
	struct piece_t p;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);

	glRotatef(view_rotx + angle, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glScalef(zoom, zoom, zoom);

	/* static content */
	glPushMatrix();
	for(i = 0; i < PIECES; i++)
	{
		p = pieces[i];
		glTranslatef(p.x, p.y, p.z);
		drawnumber(p.value);
		glTranslatef(-p.x, -p.y, -p.z);
	}
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, 10.0);
	drawbanner();
	glPopMatrix();

	SDL_GL_SwapBuffers();
}

void idle(void)
{
	view_roty += 1.5;
	zoom = zoom * 0.995;
	if(alpha < 255)
	{
		alpha += 1;
		alphaimage(texture_banner, alpha);
	}
}

/* new window size or exposure */
void reshape(int width, int height)
{
	GLfloat h = (GLfloat)height / (GLfloat)width;

	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);
}

void init(void)
{
	int i;
	struct piece_t p;
	static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);

	/* texture init */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	textureloader(GGZDATADIR "/geekgame/board.bmp", 64, 64, texture_board, 255);
	textureloader(GGZDATADIR "/geekgame/lines.bmp", 64, 64, texture_lines, 255);
	textureloader(GGZDATADIR "/geekgame/banner.bmp", 512, 128, texture_banner, 0);

	for(i = 0; i < PIECES; i++)
	{
		p.value = rand() % 2;
		p.x = (rand() % 1000 - 500) / 10.0;
		p.y = (rand() % 200 - 50) / 10.0;
		p.z = (rand() % 1000 - 500) / 10.0;
		pieces[i] = p;
	}
}

void geekgame_intro(void)
{
	int running = 1;
	SDL_Event event;
	Uint8 *keys;

	init();
	reshape(screen->w, screen->h);

	while(running)
	{
		idle();

		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_OPENGL|SDL_RESIZABLE);
					if(screen) reshape(screen->w, screen->h);
					break;
				case SDL_QUIT:
					running = 0;
					break;
			}
		}

		keys = SDL_GetKeyState(NULL);
		if(keys[SDLK_ESCAPE])
		{
			running = 0;
		}

		draw();
	}
}

