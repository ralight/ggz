/*
 * File: net_common.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 04/16/2002
 * Desc: GGZCards network common code
 * $Id: net_common.h 8427 2006-07-31 22:50:50Z jdorje $
 *
 * Contains common networking functions.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
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

#include "cards.h"
#include "ggz_dio.h"

/** @brief Reads a card from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param card A pointer to the card data. */
void read_card(GGZDataIO * dio, card_t * card);

/** @brief Wites a card to the socket.
 *  @param fd The file desciptor to which to write.
 *  @param card The card to be written. */
void write_card(GGZDataIO * dio, card_t card);

/** @brief Reads a bid from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param bid A pointer to the bid data. */
void read_bid(GGZDataIO * dio, bid_t * bid);

/** @brief Writes a bid to the socket.
 *  @param fd The file descriptor to which to read.
 *  @param bid A pointer to the bid data. */
void write_bid(GGZDataIO * dio, bid_t bid);

/** @brief Reads an opcode from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param op A pointer to the opcode data.
 *  @see enum server_msg_t, enum client_msg_t */
void read_opcode(GGZDataIO * dio, int *op);

/** @brief Writes an opcode to the socket.
 *  @param fd The file descriptor to which to write.
 *  @param op The opcode data.
 *  @see enum server_msg_t, enum client_msg_t */
void write_opcode(GGZDataIO * dio, int op);

/** @brief Reads a seat from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param seat A pointer to the seat number. */
void read_seat(GGZDataIO * dio, int *seat);

/** @brief Writes a seat number to the socket.
 *  @param fd The file descriptor to which to write.
 *  @param seat The seat number. */
void write_seat(GGZDataIO * dio, int seat);
