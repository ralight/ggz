/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 - 2003 Josef Spillner, josef@ggzgamingzone.org
 * Published under GNU GPL conditions.
 */

#ifndef METASERV_HELPER_H
#define METASERV_HELPER_H

/* Log to the logfile if logging is enabled */
void log(const char *fmt, ...);

/* Return the host part of an URI */
char *meta_uri_host_internal(const char *uri);

/* Return the port part of an URI */
int meta_uri_port_internal(const char *uri);

#endif

