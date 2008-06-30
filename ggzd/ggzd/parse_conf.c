/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 * $Id: parse_opt.c 10123 2008-06-30 20:55:07Z jdorje $
 *
 * Copyright (C) 1999-2002 Brent Hendricks.
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

#include <strings.h>

#include <ggzdmod.h>

#include "ggzdb.h"

#include "err_func.h"
#include "datatypes.h"
#include "parse_conf.h"
#include "unicode.h"

/* Log types name lookup tables*/
struct LogTypes {
	char *name;
	unsigned type;
};
static const struct LogTypes log_types[] = {
	{ "all",			GGZ_LOG_ALL },
	{ "notices",		GGZ_LOG_NOTICE },
	{ "connections",	GGZ_LOG_CONNECTION_INFO },
	{ "security",		GGZ_LOG_SECURITY },
	{ "update",			GGZ_LOG_UPDATE },
	{ "tables",			GGZ_LOG_TABLES },
	{ "admin",			GGZ_LOG_ADMIN }
};
static int num_log_types = sizeof(log_types) / sizeof(log_types[0]);

/* Parse the logging types into an unsigned int bitfield */
unsigned parse_log_types(int num, char **entry)
{
	unsigned types=0;
	int i, j;

	if(!entry) {
		return types;
	}

	for(i=0; i<num; i++) {
		for(j=0; j<num_log_types; j++)
			if(!strcasecmp(entry[i], log_types[j].name))
				break;
		if(j == num_log_types) {
			ggz_error_msg("Config: Invalid log type '%s' specified",
				entry[i]);
			ggz_error_msg("Possible types are:");
			for(j = 0; j < num_log_types; j++)
				ggz_error_msg("- %s", log_types[j].name);
		}
		else {
			ggz_debug(GGZ_DBG_CONFIGURATION,
				"%s added to log types", log_types[j].name);
			types |= log_types[j].type;
		}

		ggz_free(entry[i]);
	}

	ggz_free(entry);
	return types;
}


#ifdef DEBUG

/* Parse the debugging types into an unsigned int bitfield */
static void parse_dbg_types(int num, char **entry)
{
	int i, j;
	char *dbg_types[] = {
		GGZ_DBG_CONFIGURATION,
		GGZ_DBG_PROCESS,
		GGZ_DBG_CONNECTION,
		GGZ_DBG_CHAT,
		GGZ_DBG_TABLE,
		GGZ_DBG_PROTOCOL,
		GGZ_DBG_UPDATE,
		GGZ_DBG_MISC,
		GGZ_DBG_ROOM,
		GGZ_DBG_LISTS,
		GGZ_DBG_GAME_MSG,
		GGZ_DBG_STATS,
		GGZ_DBG_XML,
		GGZDMOD_DEBUG,
		NULL
	};



	for(i=0; i<num; i++) {
		for (j = 0; dbg_types[j]; j++)
			if(!strcasecmp(entry[i], dbg_types[j]))
				break;
				
		if (dbg_types[j] == NULL) {
			if (!strcasecmp(entry[i], "all")) {
				/* Enable all debugging types */
				for (j = 0; dbg_types[j]; j++)
					ggz_debug_enable(dbg_types[j]);
			} else {
				ggz_error_msg("Config: Invalid debug type '%s' specified",
					entry[i]);
				ggz_error_msg("Possible types are:");
				for(j = 0; dbg_types[j]; j++)
					ggz_error_msg("- %s", dbg_types[j]);
			}
		} else {
			/* FIXME: how does this work if debugging isn't set up
			   yet??? */
			ggz_debug(GGZ_DBG_CONFIGURATION,
				"%s added to debug types", dbg_types[j]);
			ggz_debug_enable(dbg_types[j]);
		}

		ggz_free(entry[i]);
	}

	ggz_free(entry);
}
#endif


/* The IANA-assigned port number for the GGZ Gaming Zone */
#define DEFAULT_GGZD_PORT 5688

/* Parse the pre-openend configuration file, close the file when done */
static void get_config_options(int ch)
{
	int intval;
	char *strval;
	char **t_list;
	int t_count = 0, i;

	/* [General] */
	if(opt.main_port == 0)
		opt.main_port = ggz_conf_read_int(ch, "General", "Port",
						  DEFAULT_GGZD_PORT);
	opt.interface = ggz_conf_read_string(ch, "General", "Interface",
					  NULL);
	opt.admin_name = ggz_conf_read_string(ch, "General", "AdminName",
					  "<unconfigured>");
	opt.admin_email = ggz_conf_read_string(ch, "General", "AdminEmail",
					  "<unconfigured>");
	opt.server_name = ggz_conf_read_string(ch, "General", "ServerName",
					   "An Unconfigured GGZ Server");

	/* Encryption in [General] */
	opt.tls_use = ggz_conf_read_int(ch, "General", "EncryptionUse", 0);
	opt.tls_password = ggz_conf_read_string(ch, "General", "EncryptionPassword", NULL);
	opt.tls_cert = ggz_conf_read_string(ch, "General", "EncryptionCert", NULL);
	opt.tls_key = ggz_conf_read_string(ch, "General", "EncryptionKey", NULL);

	/* [Directories] */
	opt.game_exec_dir = ggz_conf_read_string(ch, "Directories", "GameDir", NULL);
	opt.conf_dir = ggz_conf_read_string(ch, "Directories", "ConfDir", NULL);
	opt.data_dir = ggz_conf_read_string(ch, "Directories", "DataDir", NULL);

	/* [Database] */
	opt.db.type = ggz_conf_read_string(ch, "General", "DatabaseType", NULL);
	opt.db.option = ggz_conf_read_string(ch, "General", "DatabaseOption", NULL);
	opt.db.host = ggz_conf_read_string(ch, "General", "DatabaseHost", NULL);
	opt.db.port = ggz_conf_read_int(ch, "General", "DatabasePort", 0);
	opt.db.database = ggz_conf_read_string(ch, "General", "DatabaseName", NULL);
	opt.db.username = ggz_conf_read_string(ch, "General", "DatabaseUsername", NULL);
	opt.db.password = ggz_conf_read_string(ch, "General", "DatabasePassword", NULL);
	opt.db.hashing = ggz_conf_read_string(ch, "General", "DatabaseHashing", NULL);
	opt.db.hashencoding = ggz_conf_read_string(ch, "General", "DatabaseHashEncoding", NULL);

	/* Announcements in [General] */
	opt.announce_lan = ggz_conf_read_int(ch, "General", "AnnounceLAN", 0);
	opt.announce_metaserver = ggz_conf_read_string(ch, "General", "AnnounceMetaserver", NULL);
	opt.metausername = ggz_conf_read_string(ch, "General", "AnnounceMetaserverUsername", NULL);
	opt.metapassword = ggz_conf_read_string(ch, "General", "AnnounceMetaserverPassword", NULL);

	/* Reconfiguration in [General] */
	opt.reconfigure_rooms = ggz_conf_read_int(ch, "General", "ReconfigureRooms", 0);

	/* Unicode/policy settings in [General] */
	opt.username_policy = ggz_conf_read_string(ch, "General", "UsernamePolicy", NULL);
	strval = ggz_conf_read_string(ch, "General", "RegistrationPolicy", NULL);
	if(!ggz_strcmp(strval, "open"))
		opt.registration_policy = GGZ_REGISTRATION_OPEN;
	else if(!ggz_strcmp(strval, "confirm"))
		opt.registration_policy = GGZ_REGISTRATION_CONFIRM;
	else if(!ggz_strcmp(strval, "confirmlater"))
		opt.registration_policy = GGZ_REGISTRATION_CONFIRMLATER;
	else if(!ggz_strcmp(strval, "closed"))
		opt.registration_policy = GGZ_REGISTRATION_CLOSED;

	/* [Games] */
	ggz_conf_read_list(ch, "Games", "GameList", &g_count, &g_list);
	if(g_count == 0) {
		ggz_conf_read_list(ch, "Games", "IgnoredGames", &g_count, &g_list);
		g_count *= -1;
	}
	ggz_conf_read_list(ch, "Games", "RoomList", &r_count, &r_list);
	if(r_count == 0) {
		ggz_conf_read_list(ch, "Games", "IgnoredRooms", &r_count, &r_list);
		r_count *= -1;
	}

	/* [Files] */
	opt.motd_file = ggz_conf_read_string(ch, "Files", "MOTD", NULL);
	opt.motd_web = ggz_conf_read_string(ch, "Files", "WebMOTD", NULL);

	/* [Logs] */
	opt.dump_file = ggz_conf_read_string(ch, "Logs", "DumpFile", NULL);
	log_info.log_fname = ggz_conf_read_string(ch, "Logs", "LogFile", NULL);
	ggz_conf_read_list(ch, "Logs", "LogTypes", &t_count, &t_list);
	if(t_count > 0)
		log_info.log_types = parse_log_types(t_count, t_list);
#ifdef DEBUG
	log_info.dbg_fname = ggz_conf_read_string(ch, "Logs", "DebugFile", NULL);
	t_count = 0;
	ggz_conf_read_list(ch, "Logs", "DebugTypes", &t_count, &t_list);
	if(t_count > 0)
		parse_dbg_types(t_count, t_list);
#endif /*DEBUG*/
	strval = ggz_conf_read_string(ch, "Logs", "Facility", NULL);
	if(strval) {
		if(logfile_set_facility(strval) < 0)
			ggz_error_msg("Configuration: Invalid syslogd facility");
		ggz_free(strval);
	}
	intval = ggz_conf_read_int(ch, "Logs", "PIDInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_PID;
	else
		log_info.options |= GGZ_LOGOPT_INC_PID;
	intval = ggz_conf_read_int(ch, "Logs", "ThreadLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_THREAD_LOGS;
	else
		log_info.options |= GGZ_LOGOPT_THREAD_LOGS;
	intval = ggz_conf_read_int(ch, "Logs", "TimeInLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_TIME;
	else
		log_info.options |= GGZ_LOGOPT_INC_TIME;
	intval = ggz_conf_read_int(ch, "Logs", "GameTypeInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_GAMETYPE;
	else
		log_info.options |= GGZ_LOGOPT_INC_GAMETYPE;
#ifdef DEBUG
	log_info.verbose_updates = ggz_conf_read_int(ch, "Logs", "VerboseUpdates", 1);
#endif
	intval = ggz_conf_read_int(ch, "Logs", "UpdateInterval", 600);
	log_update_set_interval(intval);

	opt.logstatistics = ggz_conf_read_int(ch, "Logs", "Statistics", 0);

	/* Miscellaneous */
	opt.perform_lookups = ggz_conf_read_int(ch, "Miscellaneous", "HostnameLookup", 0);
	opt.ping_freq = ggz_conf_read_int(ch, "Miscellaneous", "PingFrequency", 10);
	for (i = 0; i < 4; i++) {
		char buf[1024];
		int defaults[4] = {500, 1000, 2000, 5000};

		/* The time in the config file is in milliseconds. */
		snprintf(buf, sizeof(buf), "LagClass%d", i + 1);
		opt.lag_class_time[i] = ggz_conf_read_int(ch, "Miscellaneous",
							  buf, defaults[i]);
		opt.lag_class_time[i] /= 1000.0;
	}
	opt.room_update_freq = ggz_conf_read_int(ch, "Miscellaneous",
						 "RoomUpdateFrequency", 60);

	opt.max_clients = ggz_conf_read_int(ch, "Miscellaneous", "MaxClients", 500);
	opt.max_tables = ggz_conf_read_int(ch, "Miscellaneous", "MaxTables", 150);

	if (opt.username_policy) {
		init_unicode(opt.username_policy);
	}

	ggz_conf_cleanup();
}


/* Parse options from conf file, but don't overwrite existing options*/
void parse_conf_file(void)
{
	int c_handle = -1;

	/* Use conf_parse on an approrpriate configuration file */
	if (opt.local_conf) {
		if((c_handle = ggz_conf_parse(opt.local_conf, GGZ_CONF_RDONLY)) >= 0) {
			ggz_debug(GGZ_DBG_CONFIGURATION,
				"Reading local conf file : %s", opt.local_conf);
		} else
			ggz_error_msg("WARNING:  Local conf file not found!");
	} else {
		char *global_conf = ggz_strbuild("%s/ggzd.conf", GGZDCONFDIR);

		if((c_handle = ggz_conf_parse(global_conf, GGZ_CONF_RDONLY)) >= 0)
			ggz_debug(GGZ_DBG_CONFIGURATION,
				"Reading global conf file : %s", global_conf);
		else
			ggz_error_msg("WARNING:  No configuration file loaded!");

		ggz_free(global_conf);
	}

	/* Default settings (everything != 0/NULL) */
	opt.registration_policy = GGZ_REGISTRATION_OPEN;

	/* Get options from the file */
	opt.conf_valid = false;
	if(c_handle >= 0) {
		get_config_options(c_handle);
		opt.conf_valid = true;
	}

	/* Add any defaults which were not config'ed */

	/* The default database type */
	if(!opt.db.type)
		opt.db.type = ggzdb_get_default_backend();

	/* If no game_exec_dir, default it to GGZDEXECMODDIR */
	if(!opt.game_exec_dir)
		opt.game_exec_dir = ggz_strdup(GGZDEXECMODDIR);

	/* If no conf_dir, default it to GGZDCONFDIR */
	if(!opt.conf_dir)
		opt.conf_dir = ggz_strdup(GGZDCONFDIR);

	/* If no data_dir, default it to STATEDIR */
	if (!opt.data_dir)
		opt.data_dir = ggz_strdup(GGZDSTATEDIR);
	opt.db.datadir = opt.data_dir;

	/* If no main_port, default it to 5688 (or whatever) */
	if(!opt.main_port)
		opt.main_port = DEFAULT_GGZD_PORT;

	/* Set admin stuff to ADMIN_ERR if not specified */
#define ADMIN_ERR     "<You must set this parameter in ggzd.conf>"
	if(!opt.admin_name)
		opt.admin_name = ggz_strdup(ADMIN_ERR);
	if(!opt.admin_email)
		opt.admin_email = ggz_strdup(ADMIN_ERR);

	/* Database defaults */
	if(!opt.db.hashing)
		opt.db.hashing = ggz_strdup("plain");

	if(!opt.db.hashencoding)
		opt.db.hashencoding = ggz_strdup("base64");

	if(!opt.motd_file)
		opt.motd_file = ggz_strdup("ggzd.motd");

}
