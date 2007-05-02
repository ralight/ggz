/*
 * File: err_func.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Error function prototypes
 * $Id: err_func.h 9088 2007-05-02 07:44:08Z josef $
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

#include <stdarg.h>

#include <ggz.h> /* libggz */

#include "datatypes.h"

/* Log levels for log_msg */
#define GGZ_LOG_ALWAYS		(unsigned) 0x00000000
#define GGZ_LOG_NOTICE		(unsigned) 0x00000001
#define GGZ_LOG_CONNECTION_INFO	(unsigned) 0x00000002
#define GGZ_LOG_SECURITY	(unsigned) 0x00000004
#define GGZ_LOG_TABLES		(unsigned) 0x00000008
#define GGZ_LOG_UPDATE		(unsigned) 0x00000010
#define GGZ_LOG_ADMIN		(unsigned) 0x00000020
#define GGZ_LOG_ALL		(unsigned) 0xFFFFFFFF

/* Debug levels for dbg_msg.  If you add a new entry, make sure to
   add it to dbg_types[] in parse_opt.c. */
#define GGZ_DBG_CONFIGURATION	"configuration"
#define GGZ_DBG_PROCESS		"process"
#define GGZ_DBG_CONNECTION	"connection"
#define GGZ_DBG_CHAT		"chat"
#define GGZ_DBG_TABLE		"table"
#define GGZ_DBG_PROTOCOL	"protocol"
#define GGZ_DBG_UPDATE		"update"
#define GGZ_DBG_MISC		"misc"
#define GGZ_DBG_ROOM		"room"
#define GGZ_DBG_LISTS		"lists"
#define GGZ_DBG_GAME_MSG	"game_msg"
#define GGZ_DBG_STATS		"stats"
#define GGZ_DBG_XML             "xml"

/* Logging options */
#define GGZ_LOGOPT_INC_PID	(unsigned) 0x0001
#define GGZ_LOGOPT_INC_TIME	(unsigned) 0x0002
#define GGZ_LOGOPT_USE_SYSLOG	(unsigned) 0x0004
#define GGZ_LOGOPT_THREAD_LOGS	(unsigned) 0x0008
#define GGZ_LOGOPT_INC_GAMETYPE	(unsigned) 0x0010
#define GGZ_DBGOPT_USE_SYSLOG	(unsigned) 0x0020

#define LOG_UPDATE_INTERVAL	600	/* Seconds between update log entries */
					/* Can be adjusted in ggzd.conf */

/* Logfile info */
typedef struct {
	int log_initialized;
	int syslog_facility;
	unsigned options;
	char *log_fname;			/* cleanup() */
	FILE *logfile;
	unsigned log_types;
	unsigned log_types_console;
#ifdef DEBUG
	char *dbg_fname;			/* cleanup() */
	FILE *dbgfile;
	int verbose_updates;
#endif
} LogInfo;

#define dbg_msg ggz_debug
#define err_msg ggz_error_msg
#define err_msg_exit ggz_error_msg_exit
#define err_sys ggz_error_sys
#define err_sys_exit ggz_error_sys_exit


/* Logfile info */
extern LogInfo log_info;

/* Set the syslogd facility */
int logfile_set_facility(char *);

/* Initialize the log files */
void logfile_preinitialize(void);
void logfile_initialize(void);

/* Log a normal message */
void log_msg(const unsigned, const char *, ...)
             ggz__attribute((format(printf, 2, 3)));

/* Update log handlers */
ggztime_t log_next_update_sec(void);
void log_generate_update(void);
void log_login_anon(void);
void log_login_regd(void);
void log_logout_anon(void);
void log_logout_regd(void);
void log_update_set_interval(int);
void log_create_table(void);
void log_close_table(void);
