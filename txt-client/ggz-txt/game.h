/*
 * ggz-txt - Command-line core client for the GGZ Gaming Zone.
 * This application is part of the ggz-txt-client package.
 *
 * game.h: Functions for handling game events.
 *
 * Copyright (C) 2000 Brent Hendricks
 * Copyright (C) 2001-2008 GGZ Gaming Zone Development Team
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

#include <ggzcore.h>

void game_init(GGZModule *module, GGZGameType *type, int index);

void game_quit(void);

void game_destroy(void);

void game_channel_connected(int fd);
void game_channel_ready(int fd);
