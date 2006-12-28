/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "player.h"
#include <ggz.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Memory issues: specify a maximum */
#define MAXPLAYERS 1024
#define PLAYERDB "/grubby/playerdb"

/* Globals */
int handle = -1;
Player **list = NULL;
int listsize = 0;
int dup = 0;
const char *playerdatadir = NULL;

/* Stores the data directory */
void guru_player_init(const char *datadir)
{
	playerdatadir = datadir;
}

/* Clone a player structure */
static Player *duplicate(Player *p)
{
	Player *q;

	if(!p) return NULL;
	q = (Player*)ggz_malloc(sizeof(Player));
	q->status = p->status;
	q->firstseen = p->firstseen;
	q->lastseen = p->lastseen;
	q->lastactive = p->lastactive;
	if(p->name) q->name = ggz_strdup(p->name);
	else q->name = NULL;
	if(p->realname) q->realname = ggz_strdup(p->realname);
	else q->realname = NULL;
	if(p->publicinfo) q->publicinfo = ggz_strdup(p->publicinfo);
	else q->publicinfo = NULL;
	if(p->language) q->language = ggz_strdup(p->language);
	else q->language = NULL;
	if(p->contact) q->contact = ggz_strdup(p->contact);
	else q->contact = NULL;
	q->origin = p;

	return q;
}

/* Erase a player structure again */
static void cleanup(Player *p)
{
	if(!p) return;
	if(!p->origin) return;
	if(p->name) ggz_free(p->name);
	if(p->realname) ggz_free(p->realname);
	if(p->language) ggz_free(p->language);
	if(p->publicinfo) ggz_free(p->publicinfo);
	if(p->contact) ggz_free(p->contact);
	p->origin = NULL;
	ggz_free(p);
}

/* Set duplication policy */
void guru_player_policy(int duplication)
{
	dup = duplication;
}

/* Create a new dummy player structure */
Player *guru_player_new(void)
{
	Player *p;
	p = (Player*)ggz_malloc(sizeof(Player));
	p->name = NULL;
	p->realname = NULL;
	p->language = NULL;
	p->origin = NULL;
	p->publicinfo = NULL;
	p->contact = NULL;
	p->firstseen = 0;
	p->lastseen = 0;
	p->lastactive = 0;
	p->status = STATUS_GUEST;
	return p;
}

/* Build up a structure from the given name, loading all known values */
/* FIXME: Clean up the whole function */
Player *guru_player_lookup(const char *name)
{
	int i;
	Player *p;
	int exist;
	char *path;

	if(!name) return NULL;

	/* Try to find player in cache */
	/*printf("LISTSIZE = %i\n", listsize);*/
	for(i = 0; i < listsize; i++)
		if((list[i]->name) && (!strcmp(list[i]->name, name)))
		{
			/*printf("FOUND player at %i with %s\n", list[i], list[i]->language);*/
			if(dup) return duplicate(list[i]);
			return list[i];
		}
	
	/* If not found, try to look him up */
	if(handle == -1)
	{
		path = (char*)ggz_malloc(strlen(playerdatadir) + strlen(PLAYERDB) + 1);
		strcpy(path, playerdatadir);
		strcat(path, PLAYERDB);
		handle = ggz_conf_parse(path, GGZ_CONF_CREATE | GGZ_CONF_RDWR);
		ggz_free(path);
		if(handle < 0) return NULL;
	}

	exist = ggz_conf_read_int(handle, name, "SEEN", 0);
	if(!exist) return NULL;

	/* Get him into the list */
	p = (Player*)ggz_malloc(sizeof(Player));
	p->name = ggz_strdup(name);
	p->firstseen = exist;
	p->lastseen = ggz_conf_read_int(handle, name, "LASTSEEN", 0);
	p->lastactive = ggz_conf_read_int(handle, name, "LASTACTIVE", 0);
	p->status = ggz_conf_read_int(handle, name, "STATUS", STATUS_GUEST);
	p->realname = ggz_conf_read_string(handle, name, "REALNAME", NULL);
	p->language = ggz_conf_read_string(handle, name, "LANGUAGE", NULL);
	p->publicinfo = ggz_conf_read_string(handle, name, "PUBLICINFO", NULL);
	p->contact = ggz_conf_read_string(handle, name, "CONTACT", NULL);
	p->origin = NULL;

	/* FIXME: cache is disabled until a shared memory instance is possible among the plugins */
	/*
	list = (Player**)ggz_realloc(list, ++listsize + 1);
	list[listsize - 1] = p;
	list[listsize] = NULL;
	*/

	/* If list grows too big, cut it down */
	/* TODO!!! */

	if(dup) return duplicate(p);
	return p;
}

/* Save changes to a player structure */
void guru_player_save(Player *p)
{
	if(!p) return;

	/* Saves changes in cache if another instance is present */
	if(p->origin)
	{
		cleanup(p->origin);
		*(p->origin) = *(duplicate(p));
	}

	/* Save changes on disk */
	ggz_conf_write_int(handle, p->name, "SEEN", p->firstseen);
	ggz_conf_write_int(handle, p->name, "LASTSEEN", p->lastseen);
	ggz_conf_write_int(handle, p->name, "LASTACTIVE", p->lastactive);
	ggz_conf_write_int(handle, p->name, "STATUS", p->status);
	if(p->realname) ggz_conf_write_string(handle, p->name, "REALNAME", p->realname);
	if(p->language) ggz_conf_write_string(handle, p->name, "LANGUAGE", p->language);
	if(p->publicinfo) ggz_conf_write_string(handle, p->name, "PUBLICINFO", p->publicinfo);
	if(p->contact) ggz_conf_write_string(handle, p->name, "CONTACT", p->contact);
	ggz_conf_commit(handle);
}

/* Manage to cleanup all memory associated with a player */
void guru_player_free(Player *p)
{
	cleanup(p);
}

/* Clean up all memory for this module */
void guru_player_cleanup(void)
{
	if(handle >= 0){
		ggz_conf_close(handle);
		handle = -1;
	}
}
