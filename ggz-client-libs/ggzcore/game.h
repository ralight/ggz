/*
 * File: game.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 2/28/2001
 * $Id: game.h 7784 2006-01-16 08:25:55Z jdorje $
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
#include "table.h"


struct _GGZGame *_ggzcore_game_new(void);

void _ggzcore_game_init(struct _GGZGame *game, GGZServer * server,
			GGZModule * module);
void _ggzcore_game_free(struct _GGZGame *game);

void _ggzcore_game_set_table(GGZGame *game, GGZTable *table);
void _ggzcore_game_set_seat(GGZGame * game, GGZTableSeat * seat);
void _ggzcore_game_set_spectator_seat(GGZGame * game, GGZTableSeat * seat);
void _ggzcore_game_set_player(GGZGame * game, int is_spectator,
			      int seat_num);
void _ggzcore_game_inform_chat(GGZGame * game, const char *player,
			       const char *msg);

int _ggzcore_game_is_spectator(GGZGame * game);
int _ggzcore_game_get_seat_num(GGZGame * game);
GGZRoom *_ggzcore_game_get_room(GGZGame * game);
GGZTable *_ggzcore_game_get_table(GGZGame * game);

void _ggzcore_game_set_info(GGZGame * game, int num, GGZList *infos);

/* Functions for attaching hooks to struct _GGZGame events */
int _ggzcore_game_add_event_hook_full(struct _GGZGame *game,
				      const GGZGameEvent event,
				      const GGZHookFunc func,
				      const void *data);

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

int _ggzcore_game_get_control_fd(struct _GGZGame *game);
GGZModule *_ggzcore_game_get_module(struct _GGZGame *game);

void _ggzcore_game_set_server_fd(struct _GGZGame *game, int fd);

int _ggzcore_game_launch(struct _GGZGame *game);
RETSIGTYPE _ggzcore_game_dead(int sig);

#endif /* __GAME_H_ */
