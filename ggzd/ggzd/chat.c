/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: GGZ Server
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

#include <ggzd.h>
#include <datatypes.h>
#include <chat.h>
#include <err_func.h>


/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables tables;
extern struct Users players;

/* We'll keep the chat structure local to this file */
static struct Chats chats;

/* 
 *  Local utility functions (these don't acquire the mutex since
 *  they're only called from functions that already have it) 
 */
static void chat_remove(int c_index);
static void chat_mark_all_unread(int c_index);
static void chat_status(void);

void chat_init(void) 
{
	int i;
	
	chats.count = 0;
	for (i = 0; i < MAX_CHAT_BUFFER; i++) {
		chats.info[i].p_index = -1;
	}

	chat_status();
}


int chat_add(int p_index, char* name, char* msg) 
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
			strcpy(chats.info[i].p_name, name);
			strcpy(chats.info[i].msg, msg);
			chat_mark_all_unread(i);
			break;
		}
	
	chats.count++;
	chat_status();
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


int chat_check_num_unread(int p_index)
{	
	int num_unread;

	pthread_rwlock_rdlock(&chats.lock);	
	num_unread = chats.player_unread_count[p_index];
	pthread_rwlock_unlock(&chats.lock);
	
	return num_unread;
}


void chat_mark_read(int p_index, int c_index)
{	
	pthread_rwlock_wrlock(&chats.lock);
	chats.info[c_index].unread[p_index] = 0;
	chats.info[c_index].unread_count--;
	chats.player_unread_count[p_index]--;
	chat_status();
	if (chats.info[c_index].unread_count == 0)
		chat_remove(c_index);
	pthread_rwlock_unlock(&chats.lock);
}


void chat_mark_all_read(int p_index)
{	
	int c_index;

	pthread_rwlock_wrlock(&chats.lock);
	
	if(chats.player_unread_count[p_index]) {
		/* Mark all the chats as read */
		for(c_index = 0; c_index < MAX_CHAT_BUFFER; c_index++)
			chats.info[c_index].unread[p_index] = 0;
		chats.player_unread_count[p_index] = 0;
	}

	pthread_rwlock_unlock(&chats.lock);
}


void chat_get(int c_index, char* name, char* chat)
{
	pthread_rwlock_rdlock(&chats.lock);
	strcpy(name, chats.info[c_index].p_name);
	strcpy(chat, chats.info[c_index].msg);
	pthread_rwlock_unlock(&chats.lock);
}


static void chat_remove(int c_index)
{
	dbg_msg("Removing chat %d", c_index);
	chats.info[c_index].p_index = -1;
	chats.info[c_index].unread_count = 0;
	chats.count--;
	chat_status();
}


static void chat_mark_all_unread(int c_index) 
{
	int i;
	
	/* FIXME: Currently marks chat for all logged-in users */
	pthread_rwlock_rdlock(&players.lock);
	for (i = 0; i < MAX_USERS; i++) {
		if (players.info[i].fd != -1) {
			chats.info[c_index].unread[i] = 1;
			chats.info[c_index].unread_count++;
			chats.player_unread_count[i]++;
		}
	}
	pthread_rwlock_unlock(&players.lock);
}


static void chat_status(void) 
{
	int i, j;
	
	dbg_msg("*** Chat status ***", chats.count);
	dbg_msg("%d chats in buffer", chats.count);
	for (i = 0; i < MAX_CHAT_BUFFER; i++) {
		if (chats.info[i].p_index != -1 ) {
			dbg_msg("[%s] : %s", chats.info[i].p_name, 
				chats.info[i].msg);
			dbg_msg("Unread by %d", chats.info[i].unread_count);
			for (j = 0; j < MAX_USERS; j++) 
				if (chats.info[i].unread[j])
					dbg_msg("Unread by player %d", j);
			
		}
	}
	for (i = 0; i < MAX_USERS; i++)
		if ( (j = chats.player_unread_count[i]) > 0)
			dbg_msg("Player %d has %d unread", i, j);
}	
	
