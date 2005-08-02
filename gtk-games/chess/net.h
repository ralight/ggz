/*
 * File: net.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Header for net.c
 * $Id: net.h 6903 2005-01-25 18:57:38Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include <ggzmod.h>

/* Send time option to server */
void net_send_time(int time_option);

/* Send move to server */
void net_send_move(char *move, int time);

/* Request draw */
void net_send_draw(void);

/* Call flag */
void net_call_flag(void);

/* Request update */
void net_request_update(void);

/* Update server */
void net_update_server(int time);

/* All server input will pass through here
 * Then we will filter it and send to game_update */
gboolean net_handle_input(GGZMod *mod);
