/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: NetGames
 * Date: 2/5/00
 * Desc: Functions for interfacing with chat facility
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

#include <config.h>

#include <pthread.h>
#include <string.h>

#include <datatypes.h>
#include <chat.h>


/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables game_tables;
extern struct Users players;

/* We'll keep the chat structure local to this file */
static struct Chats chats;


void chat_init(void) 
{
	int i;
	
	chats.count = 0;
	for (i = 0; i < MAX_CHAT_BUFFER; i++) {
		chats.info[i].p_index = -1;
	}
}


int chat_add(int p_index, char* msg) 
{
	int i;
	
	pthread_rwlock_wrlock(&chats.lock);
	if (chats.count == MAX_CHAT_BUFFER) {
		pthread_rwlock_unlock(&chats.lock);
		return(-1);
	}

	/* Skip over occupied buffer slots */
	for (i = 0; i < MAX_CHAT_BUFFER; i++)
		if (chats.info[i].p_index == -1) {
			chats.info[i].p_index = p_index;
			/* FIXME: Do we need player.lock here? */
			strcpy(chats.info[i].p_name, 
			       players.info[p_index].name);
			strcpy(chats.info[i].msg, msg);
			chat_mark_all_unread(i);
			break;
		}
	
	chats.count++;
	pthread_rwlock_unlock(&chats.lock);

	return 0;
}


int chat_remove(int c_index)
{
	pthread_rwlock_wrlock(&chats.lock);
	/* FIXME: Check to see if chat is present */
	chats.info[c_index].p_index = -1;
	chats.count--;
	pthread_rwlock_unlock(&chats.lock);
	
	return 0;
}


char chat_check_unread(int p_index, int c_index)
{
	char unread; 

	pthread_rwlock_rdlock(&chats.lock);
	unread = chats.info[c_index].unread[p_index];
	pthread_rwlock_unlock(&chats.lock);
	
	return unread;
}


void chat_mark_read(int p_index, int c_index)
{	
	pthread_rwlock_wrlock(&chats.lock);
	chats.info[c_index].unread[p_index] = 0;
	pthread_rwlock_unlock(&chats.lock);
}


void chat_mark_all_unread(int c_index) 
{
	int i;
	
	/* FIXME: Currently marks chat for all logged-in users */
	for (i = 0; i < MAX_USERS; i++)
		if (players.info[i].uid != NG_UID_NONE )
			chats.info[c_index].unread[i] = 1;
}

