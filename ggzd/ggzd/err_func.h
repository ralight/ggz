/*
 * File: err_func.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Error function prototypes
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

#include <easysock.h>
#include <datatypes.h>

/* Log levels for log_msg */
#define GGZ_LOG_ALWAYS		(unsigned) 0x0000
#define GGZ_LOG_NOTICE		(unsigned) 0x0001
#define GGZ_LOG_CONNECTION_INFO	(unsigned) 0x0002
#define GGZ_LOG_ALL		(unsigned) 0xFFFF

/* Debug levels for dbg_msg */
#define GGZ_DBG_CONFIGURATION	(unsigned) 0x0001
#define GGZ_DBG_PROCESS		(unsigned) 0x0002
#define GGZ_DBG_CONNECTION	(unsigned) 0x0004
#define GGZ_DBG_CHAT		(unsigned) 0x0008
#define GGZ_DBG_TABLE		(unsigned) 0x0010
#define GGZ_DBG_PROTOCOL	(unsigned) 0x0020
#define GGZ_DBG_UPDATE		(unsigned) 0x0040
#define GGZ_DBG_MISC		(unsigned) 0x0080
#define GGZ_DBG_ROOM		(unsigned) 0x0100
#define GGZ_DBG_LISTS		(unsigned) 0x0200
#define GGZ_DBG_ALL		(unsigned) 0xFFFF

/* Logging options */
#define GGZ_LOGOPT_INC_PID	(unsigned) 0x0001
#define GGZ_LOGOPT_INC_TIME	(unsigned) 0x0002
#define GGZ_LOGOPT_USE_SYSLOG	(unsigned) 0x0004
#define GGZ_LOGOPT_THREAD_LOGS	(unsigned) 0x0008
#define GGZ_DBGOPT_USE_SYSLOG	(unsigned) 0x0010

/*
 * Error printing functions based on those found in W. Richard Stevens book: 
 * "Advanced Programming in the Unix Environment"
 * 
 * err_msg : print a message
 * err_sys : print a message and the appropriate text for errno.
 * 
 * err_xxx_exit : does the above and then exits
 * 
 * dbg_msg : print a debugging message if DEBUG is defined
 */
void err_msg(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_msg_exit(const char *fmt, ...);
void err_sys_exit(const char *fmt, ...);
void dbg_msg(const unsigned, const char *fmt, ...);


/*
 * Error fprinting function for easysock lib
 */
void err_sock(const char *, const EsOpType, const EsDataType);

/* Logfile info */
extern LogInfo log_info;

/* Set the syslogd facility */
extern int logfile_set_facility(char *);

/* Initialize the log files */
extern void logfile_initialize(void);

/* Log a normal message */
extern void log_msg(const unsigned, const char *, ...);
