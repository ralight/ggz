#include "player.h"
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>

/*struct player_t
{
	char *name;
	char *realname;
	int firstseen, lastseen, lastactive;
	char *language;
	char *publicinfo;
	int status;
};

typedef struct player_t Player;*/

#define MAXPLAYERS 1024

int handle = -1;
Player **list = NULL;
int listsize = 0;
int dup = 0;

Player *duplicate(Player *p)
{
	Player *q;

	if(!p) return NULL;
	q = (Player*)malloc(sizeof(Player));
	q->status = p->status;
	q->firstseen = p->firstseen;
	q->lastseen = p->lastseen;
	q->lastactive = p->lastactive;
	if(p->name) q->name = strdup(p->name);
	else q->name = NULL;
	if(p->realname) q->realname = strdup(p->realname);
	else q->realname = NULL;
	if(p->publicinfo) q->publicinfo = strdup(p->publicinfo);
	else q->publicinfo = NULL;
	if(p->language) q->language = strdup(p->language);
	else q->language = NULL;
	q->origin = p;

	return q;
}

void cleanup(Player *p)
{
	if(!p) return;
	if(!p->origin) return;
	if(p->name) free(p->name);
	if(p->realname) free(p->realname);
	if(p->language) free(p->language);
	if(p->publicinfo) free(p->publicinfo);
	p->origin = NULL;
	free(p);
}

void guru_player_policy(int duplication)
{
	dup = duplication;
}

Player *guru_player_new()
{
	Player *p;
	p = (Player*)malloc(sizeof(Player));
	p->name = NULL;
	p->realname = NULL;
	p->language = NULL;
	p->origin = NULL;
	p->firstseen = 0;
	p->lastseen = 0;
	p->lastactive = 0;
	p->status = STATUS_GUEST;
	return p;
}

Player *guru_player_lookup(const char *name)
{
	int i;
	Player *p;
	int exist;
	char path[1024];

	if(!name) return NULL;

	/* Try to find player in cache */
	for(i = 0; i < listsize; i++)
		if((list[i]->name) && (!strcmp(list[i]->name, name)))
		{
			if(dup) return duplicate(list[i]);
			return list[i];
		}

	/* If not found, try to look him up */
	if(handle == -1)
	{
		strcpy(path, getenv("HOME"));
		strcat(path, "/.ggz/guru/playerdb");
		handle = ggzcore_confio_parse(path, GGZ_CONFIO_CREATE | GGZ_CONFIO_RDWR);
		if(handle < 0) return NULL;
	}

	exist = ggzcore_confio_read_int(handle, name, "SEEN", 0);
	if(!exist) return NULL;

	/* Get him into the list */
	p = (Player*)malloc(sizeof(Player));
	p->name = strdup(name);
	p->firstseen = exist;
	p->lastseen = ggzcore_confio_read_int(handle, name, "LASTSEEN", 0);
	p->lastactive = ggzcore_confio_read_int(handle, name, "LASTACTIVE", 0);
	p->status = ggzcore_confio_read_int(handle, name, "STATUS", STATUS_GUEST);
	p->realname = ggzcore_confio_read_string(handle, name, "REALNAME", NULL);
	p->language = ggzcore_confio_read_string(handle, name, "LANGUAGE", NULL);
	p->publicinfo = ggzcore_confio_read_string(handle, name, "PUBLICINFO", NULL);
	p->origin = NULL;

	list = (Player**)realloc(list, ++listsize + 1);
	list[listsize - 1] = p;
	list[listsize] = NULL;

	/* If list grows too big, cut it down */
	/* TODO!!! */

	if(dup) return duplicate(p);
	return p;
}

void guru_player_save(Player *p)
{
	if(!p) return;

	if(p->origin)
	{
		/* Save changes in cache */
		cleanup(p->origin);
		*(p->origin) = *(duplicate(p));
	}

	/* Save changes on disk */
	ggzcore_confio_write_int(handle, p->name, "SEEN", p->firstseen);
	ggzcore_confio_write_int(handle, p->name, "LASTSEEN", p->lastseen);
	ggzcore_confio_write_int(handle, p->name, "LASTACTIVE", p->lastactive);
	ggzcore_confio_write_int(handle, p->name, "STATUS", p->status);
	if(p->realname) ggzcore_confio_write_string(handle, p->name, "REALNAME", p->realname);
	if(p->language) ggzcore_confio_write_string(handle, p->name, "LANGUAGE", p->language);
	if(p->publicinfo) ggzcore_confio_write_string(handle, p->name, "PUBLICINFO", p->publicinfo);
	ggzcore_confio_commit(handle);
}

void guru_player_free(Player *p)
{
	cleanup(p);
}

