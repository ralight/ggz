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

/* Log levels for log_msg */
#define ALWAYS		0
#define NOTICE		1
#define CONNECTION_INFO	2

/*
 * Quick macro for determining operation failures
 */
#define FAIL(x) ((x) < 0)

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
void dbg_msg(const char *fmt, ...);


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
extern void log_msg(const int, const char *, ...);
