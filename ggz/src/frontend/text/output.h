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

/* Chat types */
typedef enum {
	CHAT_MSG,
	CHAT_PRVMSG,
	CHAT_ANNOUNCE
} ChatTypes;


/* Color Codes */
#define COLOR_BRIGHT_GREY	"\e[1m\e[30m"
#define COLOR_BRIGHT_RED	"\e[1m\e[31m"
#define COLOR_BRIGHT_GREEN	"\e[1m\e[32m"
#define COLOR_BRIGHT_ORANGE	"\e[1m\e[33m"
#define COLOR_BRIGHT_BLUE	"\e[1m\e[34m"
#define COLOR_BRIGHT_PINK	"\e[1m\e[35m"
#define COLOR_BRIGHT_PURPLE	"\e[1m\e[36m"
#define COLOR_BRIGHT_WHITE	"\e[1m\e[37m"
#define COLOR_GREY		"\e[0m\e[30m"
#define COLOR_RED		"\e[0m\e[31m"
#define COLOR_GREEN		"\e[0m\e[32m"
#define COLOR_ORANGE		"\e[0m\e[33m"
#define COLOR_BLUE		"\e[0m\e[34m"
#define COLOR_PINK		"\e[0m\e[35m"
#define COLOR_PURPLE		"\e[0m\e[36m"
#define COLOR_WHITE		"\e[0m\e[37m"

void output_banner(void);

void output_prompt(int status);

void output_text(char* fmt, ...);

void output_chat(int type, char *player, char *message);

void output_status();

void output_init(void);

void output_shutdown(void);

void output_display_help(void);
