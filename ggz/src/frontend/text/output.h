/*
 * File: output.h
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


#ifndef __OUTPUT_H__
#define __OUTPUT_H__


/* Chat types */
typedef enum {
	CHAT_MSG,
	CHAT_PRVMSG,
	CHAT_ANNOUNCE
} ChatTypes;


void output_banner(void);

void output_prompt(void);

void output_text(char* fmt, ...);

void output_chat(ChatTypes type, char *player, char *message);

void output_players(void);

void output_rooms(void);

void output_status(void);

void output_init(void);

void output_shutdown(void);

void output_display_help(void);


#endif /*__OUTPUT_H__*/
