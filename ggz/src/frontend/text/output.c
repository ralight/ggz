/*
 * File: output.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 *
 * Functions for display text/messages
 *
 * Copyright (C) 2000 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "output.h"

static struct winsize window;
static int tty_des;

void output_banner(void)
{
	fflush(NULL);
	printf("\33[f\n");
	printf("Welcome to the text-only GGZ client!\n");
	printf("--Written by Brent Hendricks & Justin Zaun  (C) 2000\n");
	printf("\n");
	fflush(NULL);
}

void output_prompt(void)
{
	fflush(NULL);
	printf("\33[%d;0f\33[2KGGZ> ",window.ws_row-1);
	fflush(NULL);
}

void output_chat(int type, char* player, char* message)
{
	if(type == 0)
	{
		printf("\33[%d;0f\33D<%s> : %s",window.ws_row-3, player, message);
	} else if (type == 1) {
		printf("\33[%d;0f\33D>%s< : %s",window.ws_row-3, player, message);
	}
	output_prompt();
}

void output_status(char* message)
{
	fflush(NULL);
	printf("\33[%d;0f\33[K%s",window.ws_row-2, message);
	fflush(NULL);
}

void output_init(void)
{
	fflush(NULL);
	printf("\33[2J");
	ioctl(tty_des, TIOCGWINSZ, &window);
	printf("\33[0;%dr",window.ws_row-3);
	fflush(NULL);
}

void output_shutdown(void)
{
	printf("\33[0;%dr",window.ws_row);
	printf("\33[2J");
	fflush(NULL);
}
