/* 
 * File: protocol.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.h 2376 2001-09-05 21:40:11Z jdorje $
 *
 * This just contains the communications protocol information.
 *
 * Copyright (C) 1999 Brent Hendricks.
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

/* this file should be identical between client and server */

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/* We don't need to define/check the protocol version since GGZ does this
   automatically.  What we may want to do is include capabilities or a minor
   version number so that slightly different protocols are supported
   compatibly. */

/* Messages from server */
typedef enum {
	WH_REQ_NEWGAME,
	WH_MSG_NEWGAME,
	WH_MSG_GAMEOVER,
	WH_MSG_PLAYERS,
	WH_MSG_HAND,
	WH_REQ_BID,
	WH_REQ_PLAY,
	WH_MSG_BADPLAY,
	WH_MSG_PLAY,
	WH_MSG_TRICK,
	WH_MESSAGE_GLOBAL,
	WH_MESSAGE_PLAYER,
	WH_REQ_OPTIONS,
	WH_MSG_TABLE
} server_msg_t;

/* Messages from client */
typedef enum {
	WH_RSP_NEWGAME,
	WH_RSP_OPTIONS,
	WH_RSP_PLAY,
	WH_RSP_BID,
	WH_REQ_SYNC
} client_msg_t;

#endif /* __PROTOCOL_H__ */
