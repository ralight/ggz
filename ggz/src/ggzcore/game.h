/*
 * File: game.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 2/28/2001
 *
 * This fils contains functions for handling games being played
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef __GAME_H__
#define __GAME_H__


#include "ggzcore.h"
#include "module.h"


struct _GGZGame* _ggzcore_game_new(void);

void _ggzcore_game_init(struct _GGZGame *game, struct _GGZModule *module);
void _ggzcore_game_free(struct _GGZGame *game);

/* Functions for attaching hooks to struct _GGZGame events */
int _ggzcore_game_add_event_hook_full(struct _GGZGame *game,
				      const GGZGameEvent event, 
				      const GGZHookFunc func,
				      void *data);

/* Functions for removing hooks from struct _GGZGame events */
int _ggzcore_game_remove_event_hook(struct _GGZGame *game,
				    const GGZGameEvent event, 
				    const GGZHookFunc func);

int _ggzcore_game_remove_event_hook_id(struct _GGZGame *game,
				       const GGZGameEvent event, 
				       const unsigned int hook_id);

int _ggzcore_game_data_is_pending(struct _GGZGame *game);
int _ggzcore_game_read_data(struct _GGZGame *game);
int _ggzcore_game_write_data(struct _GGZGame *game);

int _ggzcore_game_get_fd(struct _GGZGame *game);
struct _GGZModule* _ggzcore_game_get_module(struct _GGZGame *game);

int _ggzcore_game_launch(struct _GGZGame *game);
int _ggzcore_game_join(struct _GGZGame *game);
int _ggzcore_game_send_data(struct _GGZGame *game, char *buffer);
			    

#endif /* __GAME_H_ */

