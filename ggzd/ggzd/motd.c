/*
 * File: motd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 02/05/2000
 * Desc: Handle message of the day functions
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

#include <datatypes.h>
#include <err_func.h>

/* MOTD info */
MOTDInfo motd_info;

/* We need to get the game_dir */
extern Options opt;

#define MOTD_MAX 1024		/* Warning: changing this may break clients! */

/* Read the motd file */
void motd_read_file(void)
{
#ifdef 0
	char *fullpath;
	FILE *motd_file;
	char motd_temp[MOTD_MAX+1];
	int motd_size;
	char *motd_final;

	/* If it's an absolute path already, we don't need to add game_dir */
	if(motd_info.motd_file[0] == '/')
		fullpath = motd_info.motd_file;
	else {
		if((fullpath = malloc(strlen(motd_info.motd_file) +
				      strlen(opt.game_dir) + 2)) == NULL)
			err_sys_exit("malloc error in motd_read_file()");
		sprintf(fullpath, "%s/%s", opt.game_dir, motd_info.motd_file);
	}

	/* Try to open the file */
	motd_info.use_motd = 1;
	if((motd_file = fopen(fullpath, "r")) == NULL) {
		err_msg("MOTD file not found");
		motd_info.use_motd = 0;
		return;
	}

	/* Read the whole file in one chunk, up to MOTD_MAX */
	motd_size = fread(motd_temp, 1, MOTD_MAX, motd_file);
	if(motd_size > 0) {
		/* Make a string out of it */
		motd_temp[motd_size] = '\0';

		/* Allocate and fill a permanent buffer for it */
		if((motd_final = malloc(motd_size+1)) == NULL)
			err_sys_exit("malloc error in motd_read_file()");
		strcpy(motd_final, motd_temp);
		motd_info.motd_text = motd_final;
	} else
		motd_info.use_motd = 0;

	fclose(motd_file);
	dbg_msg("Read MOTD file, %d bytes", motd_size);
#endif
}
