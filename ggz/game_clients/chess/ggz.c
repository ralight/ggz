/*
 * File: ggz.c
 * Author: Ismael Orenstein
 * Project: libggzmod
 * Date: 09/17/2000
 * Desc: Utilities functions for game modules
 *
 * Copyright (C) 2001 Ismael Orenstein.
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

/* Use this function to connect to the game server 
 * name -> the game name, as described in the .dsc file
 * This function returns the file description for the connection, 
 * that should be used in all socket calls */
int ggz_connect(char *name) {
	char fd_name[30];
  int fd;
	struct sockaddr_un addr;
						 
	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/%s.%d", name, getpid());
		 
	if ( (fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);
		 
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
					 
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);

  return fd;
}    
