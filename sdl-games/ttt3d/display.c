/*
 * GGZ Gaming Zone TicTacToe 3D
 * Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
 * $Id: display.c 6759 2005-01-20 05:17:31Z jdorje $
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

#include "display.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>

#define texture_none 0
#define texture_board 1
#define texture_lines 2
#define texture_cursor 3
#define texture_figure 4

int cursor = -1;
int cursormove = 0;
static int usetextures = 1;
int usetexturesmove = 0;
int movemove = 0;

char board[9];

#define TEXTURESIZE 1048576
#define TEXTURECOUNT 8

unsigned char texture[TEXTURESIZE][4];
long texturevalue[TEXTURECOUNT][4];
static int maxtexturecount = 0;
static int maxtexture = 0;

GLfloat view_rotx = 0.0, view_roty = 0.0;
GLfloat angle = 0.0, introangle = 0.0;

static void teximage(int tex)
{
	long n = -1;
	long i;

	for(i = 0; i < maxtexturecount; i++) if(texturevalue[i][3] == tex) n = i;
	if(n == -1) return;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturevalue[n][0], texturevalue[n][1],
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture[texturevalue[n][2]]);
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
		if(usetextures) glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);
	}

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

	if(tex != texture_none)
	{
		glDisable(GL_TEXTURE_2D);
	}

	glPopMatrix();
}

static void dodrawstatic()
{
	static GLfloat red[4] = {1.0, 0.1, 0.0, 1.0};
	static GLfloat red2[4] = {1.0, 1.0, 0.2, 1.0};

	/* (1) draw the board */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);

	drawbox(10.0, 1.0, 10.0, texture_board);

	/* (2) draw the lines on the board */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red2);

	glPushMatrix();
	glTranslatef(1.7, 0.7, 0.0);
	drawbox(0.1, 0.1, 10.0, texture_lines);
	glTranslatef(-3.4, 0.0, 0.0);
	drawbox(0.1, 0.1, 10.0, texture_lines);
	glTranslatef(1.7, 0.0, 0.0);

	glTranslatef(0.0, 0.0, 1.7);
	drawbox(10.0, 0.1, 0.1, texture_lines);
	glTranslatef(0.0, -0.0, -3.4);
	drawbox(10.0, 0.1, 0.1, texture_lines);
	glTranslatef(0.0, 0.0, 1.7);
	glPopMatrix();
}

static void dodrawdynamic(void)
{
	int i;
	static GLfloat red3[4] = {0.0, 0.0, 0.9, 1.0};
	static GLfloat red4[4] = {0.2, 0.8, 0.1, 1.0};
	static GLfloat red5[4] = {0.7, 0.1, 0.1, 1.0};

	/* (3) draw the crosses */
	for(i = 0; i < 9; i++)
	{
		if(board[i] != ' ')
		{
			if(board[i] == 'x') glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red3);
			else glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red4);

			glPushMatrix();
			glTranslatef(-3.3 + (i / 3) * 3.3, 2.0, -3.3 + (i % 3) * 3.3);
			drawbox(1.0, 1.0, 3.0, texture_figure);
			drawbox(1.0, 3.0, 1.0, texture_figure);
			drawbox(3.0, 1.0, 1.0, texture_figure);
			glPopMatrix();
		}
	}

	/* (5) draw cursor */
	if(cursor >= 0)
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red5);

		glPushMatrix();
		glTranslatef(-3.3 + (cursor / 3) * 3.3, 1.0, -3.3 + (cursor % 3) * 3.3);
		drawbox(3.0, 0.1, 3.0, texture_cursor);
		glPopMatrix();
	}
}

static void textureloader(char *filename, int width, int height, int tex)
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
					texture[maxtexture][3] = 255;
					maxtexture++;
				}
			}
		fclose(texfile);
	}
}

void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);

	glRotatef(view_rotx + angle, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);

	/* static content */
	glPushMatrix();
	dodrawstatic();
	glPopMatrix();

	/* dynamic content */
	dodrawdynamic();

	glPopMatrix();

	SDL_GL_SwapBuffers();
}

void idle(void)
{
	int i;

	if(introangle < 50.0) introangle += 0.1;
	if((introangle >= 50.0) && (angle < 45.0)) angle += 0.1;

	if(cursormove > 0) cursormove--;
	if(cursormove == 1)
	{
		for(i = 0; (i < 1) || ((board[cursor] != ' ') && (i < 9)); i++)
			cursor = (cursor + 1) % 9;
	}

	if(usetexturesmove > 0) usetexturesmove--;
	if(usetexturesmove == 1)
	{
		usetextures = !usetextures;
	}

	if(movemove > 0) movemove--;
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
	textureloader(GGZDATADIR "/ttt3d/board.bmp", 64, 64, texture_board);
	textureloader(GGZDATADIR "/ttt3d/lines.bmp", 64, 64, texture_lines);
	textureloader(GGZDATADIR "/ttt3d/cursor.bmp", 64, 64, texture_cursor);
	textureloader(GGZDATADIR "/ttt3d/figure.bmp", 32, 32, texture_figure);
}

