/*
 * File: net.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Header for net.c
 * $Id: net.h 4491 2002-09-09 04:51:32Z jdorje $
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
void net_handle_input(gpointer data, int fd, GdkInputCondition cond);
