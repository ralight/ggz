/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions.
 */

#ifndef METASERV_HELPER_H
#define METASERV_HELPER_H

/* Log to the logfile if logging is enabled */
void logline(const char *fmt, ...);

/* Duplicate a string, doing xml-quoting if necessary */
char *xml_strdup(const char *s, int attribute, int url);

#endif

