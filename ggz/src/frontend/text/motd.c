/* 
 * File: motd.c
 * Author: Justin Zaun
 * Project: GGZ text Client
 * $Id: motd.c 4851 2002-10-10 17:42:40Z jdorje $
 *
 * Copyright (C) 2000 Justin Zaun.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "output.h"
#include "motd.h"

static char *color_sequences[] = { "\e[0m\e[37m" /* 0 */ ,
				   "\e[0m\e[34m" /* 1 */ ,
				   "\e[0m\e[31m" /* 2 */ ,
				   "\e[1m\e[33m" /* 3 */ ,
				   "\e[0m\e[36m" /* 4 */ ,
				   "\e[0m\e[33m" /* 5 */ ,
				   "\e[1m\e[30m" /* 6 */ ,
				   "\e[1m\e[34m" /* 7 */ ,
				   "\e[0m\e[34m" /* 8 */ ,
				   "\e[0m\e[37m" /* 9 */ };

void motd_print_line(const char *line)
{
	char out[1024];
	int lindex = 0;
	int oindex = 0;
	int color = 0;	/* Black */

	while (line[lindex] != '\0') {
		if (line[lindex] == '%') {
			lindex++;
			if (line[lindex] == 'c') {
				lindex++;
				color = atoi(&line[lindex]);
				if (color >= 0 && color <= 9) {
					strcat(out + oindex,
					       color_sequences[color]);
					oindex +=
						strlen(color_sequences
						       [color]);
					lindex++;
				} else {
					lindex--;
					lindex--;
				}
			} else {
				lindex--;
			}
		}
		out[oindex] = line[lindex];
		lindex++;
		oindex++;

		if (out[oindex - 1] == '\n') {
			/* Remove the \n as we add one in output_text */
			out[oindex - 1] = '\0';
			output_text("%s", out);
			oindex = 0;
		}
	}

	if (oindex > 0) {
		/* Append any trailing characters that aren't
		   part of a line. */
		out[oindex] = '\0';
		output_text("%s", out);
	}
}
