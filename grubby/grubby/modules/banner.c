/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/*
For displaying a banner to players on entry.
Hosts/admins have the ability to set the banner.

/msg grubby show
/msg grubby banner this is the banner
/msg grubby nobanner

NOTE - using mysql here is a hack. Modules should have the ability to query
the player type.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "player.h"
#include "i18n.h"
#include <time.h>
#include <ggz.h>

#include <mysql/mysql.h>

/* Configuration file for banner */
#define BANNERCONF "/.ggz/grubby/modbanner.rc"

static char *banner = NULL;
static MYSQL *conn = NULL;

/* Dummy init function */
void gurumod_init(const char *datadir)
{
	int handle;
	char *home = NULL;
	char *path = NULL;
	char *sql_host;
	char *sql_user;
	char *sql_password;
	char *sql_database;
	my_bool reconnect = 1;

	if(conn) return;

	home = getenv("HOME");
	path = (char*)ggz_malloc(strlen(home) + strlen(BANNERCONF) + 1);
	strcpy(path, home);
	strcat(path, BANNERCONF);
	handle = ggz_conf_parse(path, GGZ_CONF_RDONLY);
	ggz_free(path);
	if(handle < 0) return;

	sql_host = ggz_conf_read_string(handle, "banner", "sql_host", "localhost");
	sql_user = ggz_conf_read_string(handle, "banner", "sql_user", "ggz");
	sql_password = ggz_conf_read_string(handle, "banner", "sql_password", NULL);
	sql_database = ggz_conf_read_string(handle, "banner", "sql_database", "localhost");

	mysql_library_init();
	conn = mysql_init(NULL);
	/*
	 * MYSQL_OPT_RECONNECT set to true here enables automatic reconnections to the
	 * server if the connection is lost. This has been disabled by default since
	 * mysql 5.0.3. This option requires 5.0.13.
	 */
	mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect);
	mysql_real_connect(conn, sql_host, sql_user, sql_password, sql_database, 0, NULL, 0);

	ggz_free(sql_host);
	ggz_free(sql_user);
	ggz_free(sql_password);
	ggz_free(sql_database);
	ggz_conf_close(handle);
}

Guru *gurumod_exec(Guru *message)
{
	int i;
	static char *info = NULL;
	static int showbanner = 0;

	Player *p;
	int firsttime;

	static char query[1024];
	long perms = 0L;
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;

	if(!conn) return NULL;
	if(!info) info = (char*)ggz_malloc(1024);

	if(conn && message && message->player){
		snprintf(query, sizeof(query), "SELECT `perms` FROM `users` WHERE `username` = LOWER('%s') LIMIT 1", message->player);
		rc = mysql_query(conn, query);
		if(!rc){
			res = mysql_store_result(conn);
			if(res){
				if(mysql_num_rows(res) == 1) {
					row = mysql_fetch_row(res);
					perms = atol(row[0]);
				}
				mysql_free_result(res);
			}
		}
	}

	p = guru_player_lookup(message->player);
	firsttime = 0;
	if(!p)
	{
		firsttime = 1;
		p = guru_player_new();
		p->name = message->player;
		p->firstseen = time(NULL);
	}

	if((perms == 0x19f || perms == 0x187) && message->type == GURU_PRIVMSG){ /* Admin || Host */
		guru_player_free(p);
		i = 0;
		while((message->list) && (message->list[i])) i++;

		if(i == 2){
			// Clear banner
			if(!strcmp(message->list[1], "nobanner")){
				showbanner = 0;
				snprintf(info, 1024, "Banner removed");
				message->message = info;
				return message;
			}else if(!strcmp(message->list[1], "show")){
				if(banner){
					snprintf(info, 1024, "Banner: %s", banner);
				}else{
					snprintf(info, 1024, "No banner");
				}
				message->message = info;
				return message;
			}
			return NULL;
		}else if(i > 2){
			// Set banner
			if(!strcmp(message->list[1], "banner")){
				if(!banner) banner = ggz_malloc(1024);
				if(!banner){
					snprintf(info, 1024, "Unable to set banner, sorry");
					message->message = info;
					return message;
				}
				showbanner = 1;
				snprintf(banner, 1024, "%s", message->message+strlen(message->list[0])+strlen(message->list[1])+2);
				snprintf(info, 1024, "Banner set to \"%s\"", banner);
				message->message = info;
				return message;
			}
			return NULL;
		}
	}


	if(!message->message) {
		/* Send banner on entry */
		if(message->type == GURU_ENTER && banner && showbanner && (firsttime || time(NULL)-p->lastseen > 600)){
			p->lastseen = time(NULL);
			guru_player_save(p);
			message->message = banner;
			message->type = GURU_PRIVMSG;
			guru_player_free(p);
			return message;
		}
	}
	guru_player_free(p);

	return NULL;
}

void gurumod_finish()
{
	if(banner){
		ggz_free(banner);
		banner = NULL;
	}
	if(conn){
		mysql_close(conn);
		mysql_library_end();
		conn = NULL;
	}
}

