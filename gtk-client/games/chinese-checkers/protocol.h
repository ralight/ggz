/*
 * File: protocol.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers game module
 * Date: 01/01/2001
 * Desc: Protocol definition
 *
 * Copyright (C) 2001 Richard Gade.
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


/* Chinese Checkers protocol */
/* Messages from server */
#define CC_MSG_SEAT	0
#define CC_MSG_PLAYERS	1
#define CC_MSG_MOVE	2
#define CC_MSG_GAMEOVER	3
#define CC_MSG_SYNC	4
#define CC_REQ_MOVE	10
#define CC_RSP_MOVE	20

/* Move errors */
#define CC_ERR_STATE	-1
#define CC_ERR_TURN	-2
#define CC_ERR_BOUND	-3
#define CC_ERR_INVALID	-4
#define CC_ERR_FULL	-5

/* Messages from client */
#define CC_SND_MOVE	0
#define CC_REQ_SYNC	10
#define CC_REQ_NEWGAME	11

/* Chinese Checkers game states */
#define CC_STATE_INIT		0
#define CC_STATE_WAIT		1
#define CC_STATE_PLAYING	2
#define CC_STATE_DONE		3

/* Chinese Checkers game events */
#define CC_EVENT_LAUNCH	0
#define CC_EVENT_JOIN	1
#define CC_EVENT_LEAVE	2
#define CC_EVENT_MOVE	3
