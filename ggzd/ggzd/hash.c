/*
 * File: hash.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/08/2000
 * Desc: Functions for handling the player name hash tables
 * $Id: hash.c 5928 2004-02-15 02:43:16Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "ggzd.h"
#include "datatypes.h"
#include "err_func.h"


/* Internal variables */
typedef struct HashList {
	struct HashList *next;
	char *name;
	GGZPlayer* player;
} HashList;
static HashList *hash_list[HASH_NUM_LISTS];
static pthread_rwlock_t hash_list_lock[HASH_NUM_LISTS];


/* Internal functions */
static unsigned hash_pjw(const char *name);
static void hash_player_lowercase(const char *orig, char *buf);

/* Debugging stuff */
#ifdef DEBUG
static int hash_list_entries[HASH_NUM_LISTS];
static void hash_dump_stats(void);
static int hash_use_count=0;
static pthread_rwlock_t hash_use_count_lock;
#endif


/* Initialize the hash table lock structures */
void hash_initialize(void)
{
	int i;

	for(i=0; i<HASH_NUM_LISTS; i++)
		pthread_rwlock_init(&hash_list_lock[i], NULL);

#ifdef DEBUG
	pthread_rwlock_init(&hash_use_count_lock, NULL);
#endif
}


/* Adds a player name to the appropriate hash list */
int hash_player_add(const char *orig_name, GGZPlayer* player)
{
	unsigned hash_num;
	HashList *hl;
	bool not_exist = true;
	char name[MAX_USER_NAME_LEN + 1];

	/* Lowercase player names for hashing */
	hash_player_lowercase(orig_name, name);

	/* Pick a list */
	hash_num = hash_pjw(name) % HASH_NUM_LISTS;

	/* Check to ensure that the name isn't in use */
	pthread_rwlock_wrlock(&hash_list_lock[hash_num]);
	hl = hash_list[hash_num];
	while(hl) {
		if (strcasecmp(name, hl->name) == 0) {
			not_exist = false;
			break;
		}
		hl = hl->next;
	}

	/* If it doesn't exist, we can add it */
	if (not_exist) {
		/* Allocate space for hash entry and name */
		hl = ggz_malloc(sizeof(HashList));
		hl->name = ggz_strdup(name);

		/* Set the data items (name was set above) */
		hl->player = player;
		hl->next = hash_list[hash_num];

		/* And put this at the head of the list */
		hash_list[hash_num] = hl;

#ifdef DEBUG
		/* Finally, track how full our lists are */
		hash_list_entries[hash_num] ++;
		pthread_rwlock_unlock(&hash_list_lock[hash_num]);
		pthread_rwlock_rdlock(&hash_list_lock[hash_num]);
		pthread_rwlock_wrlock(&hash_use_count_lock);
		if(++hash_use_count >= HASH_DEBUG_FREQ) {
			hash_dump_stats();
			hash_use_count = 0;
		}
		pthread_rwlock_unlock(&hash_use_count_lock);
#endif /*DEBUG*/
	}

	/* Unlock this hash list */
	pthread_rwlock_unlock(&hash_list_lock[hash_num]);

	/* Return status */
	return not_exist;
}


/* Lookup a player name in the hash and return a pointer to him */
GGZPlayer* hash_player_lookup(const char *orig_name)
{
	unsigned hash_num;
	char name[MAX_USER_NAME_LEN + 1];
	HashList *hl;
	GGZPlayer* player = NULL;

	if(name == NULL)
		return NULL;

	/* Convert name to lowercase for comparisons */
	hash_player_lowercase(orig_name, name);

	/* Pick a list */
	hash_num = hash_pjw(name) % HASH_NUM_LISTS;

	/* Find the player name in this list */
	pthread_rwlock_rdlock(&hash_list_lock[hash_num]);
	hl = hash_list[hash_num];
	while(hl) {
		if (strcasecmp(name, hl->name) == 0) {
			player = hl->player;
			break;
		}
		hl = hl->next;
	}

	/* Lock the player before we return */
	if (player)
		pthread_rwlock_wrlock(&player->lock);

	pthread_rwlock_unlock(&hash_list_lock[hash_num]);

	return player;
}


/* Remove a player name from the hash tables */
void hash_player_delete(const char *orig_name)
{
	unsigned hash_num;
	char name[MAX_USER_NAME_LEN + 1];
	HashList *hl, *hp;

	/* Convert name to lowercase for comparisons */
	hash_player_lowercase(orig_name, name);

	/* Pick a list */
	hash_num = hash_pjw(name) % HASH_NUM_LISTS;

	/* Find the name in this list */
	pthread_rwlock_wrlock(&hash_list_lock[hash_num]);
	hp = NULL;
	hl = hash_list[hash_num];
	while(hl) {
		if (strcasecmp(name, hl->name) == 0) {
			break;
		}
		hp = hl;
		hl = hl->next;
	}

	/* Verrrry odd if it's not here, make a note */
	if(hl == NULL) {
		pthread_rwlock_unlock(&hash_list_lock[hash_num]);
		err_msg("*NOTICE* - Player name hash table is corrupt!");
		return;
	}

	/* Lose this entry */

	/* First, set the previous entry to point to our next */
	if(hp == NULL)
		hash_list[hash_num] = hl->next;
	else
		hp->next = hl->next;

#ifdef DEBUG
	/* Next, track how full our lists are */
	hash_list_entries[hash_num] --;
	pthread_rwlock_unlock(&hash_list_lock[hash_num]);
	pthread_rwlock_rdlock(&hash_list_lock[hash_num]);
	pthread_rwlock_wrlock(&hash_use_count_lock);
	if(++hash_use_count >= HASH_DEBUG_FREQ) {
		hash_dump_stats();
		hash_use_count = 0;
	}
	pthread_rwlock_unlock(&hash_use_count_lock);
#endif /*DEBUG*/

	/* And clear the lock */
	pthread_rwlock_unlock(&hash_list_lock[hash_num]);

	/* Finally we can free up the memory for our entry */
	ggz_free(hl->name);
	ggz_free(hl);
}


/* A nice hash generator, credit to Aho/Holub */
#define NBITS			(sizeof(int) * 8)
#define SEVENTY_FIVE_PERCENT	((int) (NBITS * .75))
#define TWELVE_PERCENT		((int) (NBITS * .125))
#define HIGH_BITS		( ~( (unsigned) (~0) >> TWELVE_PERCENT))
static unsigned hash_pjw(const char *name)
{
	unsigned h=0;
	unsigned g;

	for(; *name; ++name) {
		h = (h << TWELVE_PERCENT) + *name;
		if((g = (h & HIGH_BITS)))
			h = (h ^ (g >> SEVENTY_FIVE_PERCENT)) & ~HIGH_BITS;
	}

	return h;
}


/* Lower-case a player's name so that it can be case-insensitive.
 * FIXME: this probably isn't necessary since we use strcasecmp. */
static void hash_player_lowercase(const char *src, char *dst)
{
	int i;

	/* Convert name to lowercase for comparisons */
	for(i = 0; src[i] != '\0'; i++)
		dst[i] = tolower(src[i]);
	dst[i] = '\0';
}


#ifdef DEBUG
/* This dumps some nice hash debugging stats */
static void hash_dump_stats(void)
{
	int i, max=0, min=999999, tot=0, num;
	float avg;

	dbg_msg(GGZ_DBG_MISC, "=-=-=- Hash table stats -=-=-=");
	for(i=0; i<HASH_NUM_LISTS; i++) {
		num = hash_list_entries[i];
		if(num > max)
			max = num;
		if(num < min)
			min = num;
		tot += num;
		dbg_msg(GGZ_DBG_LISTS, "  List %d has %d entries", i, num);
	}
	avg = tot / (float)HASH_NUM_LISTS;

	dbg_msg(GGZ_DBG_MISC, "  Total hash entries = %d", tot);
	dbg_msg(GGZ_DBG_MISC, "  Minimum chain length = %d", min);
	dbg_msg(GGZ_DBG_MISC, "  Maximum chain length = %d", max);
	dbg_msg(GGZ_DBG_MISC, "  Average chain length = %0.2f", avg);
	dbg_msg(GGZ_DBG_MISC, "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
}
#endif /*DEBUG*/
