/*
 * File: nets.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 8/27/01
 * Desc: Functions for handling network IO
 *
 * Copyright (C) 1999-2001 Brent Hendricks.
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


#ifndef _GGZ_NET_H
#define _GGZ_NET_H

#include <config.h>
#include <players.h>
#include <login.h>

/* Read data from socket for a particu;ar player */
int net_read_data(GGZPlayer* player);

int net_send_login(GGZLoginType type, GGZPlayer *player, char status, char *password);
int net_send_motd(GGZPlayer *player);
int net_send_chat(GGZPlayer *player, unsigned char opcode, char *name, char *msg);
int net_send_chat_result(GGZPlayer *player, char status);
int net_send_logout(GGZPlayer *player, char status);

#endif /* _GGZ_NET_H */
