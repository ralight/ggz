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
#include "output.h"

#include <stdio.h>


void output_banner(void)
{
	printf("\n");
	printf("Welcome to the text-only GGZ client!\n");
	printf("--Written by Brent Hendricks (C) 2000\n");
	printf("\n");
}


void output_prompt(void)
{
	fflush(NULL);
	printf("command> ");
	fflush(NULL);
}


void output_chat(char* player, char* message)
{
	printf("\n <%s> : %s\n", player, message);
}
