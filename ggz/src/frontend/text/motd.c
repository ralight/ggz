/*
 * File: motd.c
 * Author: Justin Zaun
 * Project: GGZ text Client
 *
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


void motd_print_line(char *line)
{                                       
        char out[1024];
        /*char outline[1024];*/
        int lindex=0;
        int oindex=0;                          
        int color_index=0; /* Black */
        int letter;
                        
        while(line[lindex] != '\0')
        {
                if (line[lindex] == '%')
                {
                        lindex++;
                        if (line[lindex] == 'c')
                        {
                                lindex++;
                                letter = atoi(&line[lindex]);
                                if ((letter>=0) && (letter<=9))
                                {
                                        color_index=atoi(&line[lindex]);
					if (color_index == 0)
					{
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '7';
						out[oindex+8] = 'm';
					} else if (color_index == 1)
					{
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '4';
						out[oindex+8] = 'm';
					} else if (color_index == 2) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '1';
						out[oindex+8] = 'm';
					} else if (color_index == 3) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '1';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '3';
						out[oindex+8] = 'm';
					} else if (color_index == 4) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '6';
						out[oindex+8] = 'm';
					} else if (color_index == 5) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '3';
						out[oindex+8] = 'm';
					} else if (color_index == 6) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '1';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '0';
						out[oindex+8] = 'm';
					} else if (color_index == 7) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '1';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '4';
						out[oindex+8] = 'm';
					} else if (color_index == 8) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '4';
						out[oindex+8] = 'm';
					} else if (color_index == 9) {
						out[oindex] = '\e';
						out[oindex+1] = '[';
						out[oindex+2] = '0';
						out[oindex+3] = 'm';
						out[oindex+4] = '\e';
						out[oindex+5] = '[';
						out[oindex+6] = '3';
						out[oindex+7] = '7';
						out[oindex+8] = 'm';
					} 
					oindex = oindex+9;
                                        lindex++;
                                }else {
                                        lindex--;
                                        lindex--;
                                }
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
	/* Remove the \n as we addone in output_text */
        out[oindex-1]='\0';
	output_text(out);
}


