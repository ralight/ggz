/* 
 * File: ggzmod-ggz.h
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/20/02
 * Desc: GGZ game module functions, GGZ side
 * $Id: ggzmod-ggz.h 4968 2002-10-21 04:27:00Z jdorje $
 *
 * This file contains the GGZ-only interface for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
 * and game servers.  This file provides a unified interface that can be
 * used at both ends.
 *
 * Copyright (C) 2002 GGZ Development Team.
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


#ifndef __GGZMOD_GGZ_H__
#define __GGZMOD_GGZ_H__

typedef enum {
  /* Sit down (stop spectatin; join a seat) */
  GGZMOD_TRANSACTION_SIT,

  /* Stand up (leave your seat; become a spectator) */
  GGZMOD_TRANSACTION_STAND,

  /* Boot a player */
  GGZMOD_TRANSACTION_BOOT,

  /* Replace a bot/reserved seat with an open one */
  GGZMOD_TRANSACTION_OPEN,

  /* Put a bot into an open seat */
  GGZMOD_TRANSACTION_BOT
} GGZModTransaction;

typedef void (*GGZModTransactionHandler) (GGZMod * mod, GGZModTransaction t,
					  void *data);

void ggzmod_set_transaction_handler(GGZMod * ggzmod, GGZModTransaction t,
				    GGZModTransactionHandler func);

/** @brief Set the module executable, pwd, and arguments
 *
 *  GGZmod must execute and launch the game to start a table; this
 *  function allows ggz to specify how this should be done.
 *  @note This should not be called by the table, only ggz.
 *  @param ggzmod The GGZmod object.
 *  @param pwd The working directory for the game, or NULL.
 *  @param args The arguments for the program, as needed by exec.
 *  @note The pwd directory must already exist.
 */
void ggzmod_set_module(GGZMod * ggzmod, const char *pwd, char **args);
		       

/** @brief Set the fd of the game server connection
 *  @param ggzmod The GGZMod object.
 *  @return The server connection fd
 */
void ggzmod_set_server_fd(GGZMod * ggzmod, int fd);

/** @brief Set data about which seat at which this ggzmod is sitting.
 *
 *  The GGZ client can use this function to set data about this client.
 *  @param ggzmod The GGZMod object.
 *  @param is_spectator TRUE iff the player is a spectator.
 *  @param seat_num The seat or spectator seat number.
 *  @return 0 on success, negative on error.
 */
int ggzmod_set_player(GGZMod *ggzmod,
		      const char *my_name,
		      int is_spectator, int seat_num);

/** @brief Set seat data.
 *
 *  The GGZ client can use this function to set data about
 *  a seat.
 *  @param seat The new seat structure (which includes seat number).
 *  @return 0 on success, negative on failure.
 */
int ggzmod_set_seat(GGZMod *ggzmod, GGZSeat * seat);

/** @brief Set spectator data.
 *
 *  The GGZ client can use this function to set data about a spectator seat.
 *  @param seat The new spectator seat data.
 *  @return 0 on success, negative on failure.
 */
int ggzmod_set_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat * seat);

#endif /* __GGZMOD_GGZ_H__ */
