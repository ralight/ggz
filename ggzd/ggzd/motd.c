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
	char *fullpath;
	FILE *motd_file;
	char line[128];
	int lines;

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

	/* Read the file one line at a time (up to 15) */
	lines = 0;
	while(fgets(line, 128, motd_file) && (lines < 15)) {
		if((motd_info.motd_text[lines] = malloc(strlen(line)+1)) ==NULL)
			err_sys_exit("malloc error in motd_read_file()");
		strcpy(motd_info.motd_text[lines], line);
		lines++;
	}
	motd_info.motd_lines = lines;

	fclose(motd_file);
	dbg_msg("Read MOTD file, %d lines", lines);
}
