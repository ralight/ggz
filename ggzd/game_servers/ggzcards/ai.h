/* 
 * File: ai.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.h 4398 2002-09-03 04:55:19Z jdorje $
 *
 * This file contains the frontend for GGZCards' AI module.
 * Specific AI's are in the ai/ directory.  This file contains an array
 * of AI function sets.  Each game specifies its AI.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#ifndef __AI_H__
#define __AI_H__

#include "types.h"

typedef struct ai_module_t {
	char *name;
	char *game;
	char *path;
	struct ai_module_t *next;
} ai_module_t;

/* Launches an external AI program. */
void start_ai(player_t p, const ai_module_t *module);

/* Kills and collects a spawned AI player; should be called
   when the server is ready to exit. */
void stop_ai(player_t p);

/* Kill the current AI and restart a "random" AI in its place.
   This should be called when there's a fatal error with the AI. */
void restart_ai(player_t p);

#ifdef DEBUG
/* Handle incoming data from the AI's stderr. */
void handle_ai_stderr(player_t ai);
#endif

/* Reads in the *.aispec AI module files. */
void read_ai_modules(const char *exec_cmd);

/* Once the game has been chosen, picks the most appropriate AI
   module for the game.  (Eventually the user will be able to
   choose between multiple modules.) */
ai_module_t *choose_ai_module(void);

#endif /* __AI_H__ */
