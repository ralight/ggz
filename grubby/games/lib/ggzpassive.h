/*
 * Noninteractive game module support for Guru
 * Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GGZ_PASSIVE_H
#define GGZ_PASSIVE_H

/* GGZPassive is a main loop implementation for GGZ game clients.
 * Games which are written without a toolkit which already provides a mainloop
 * can use GGZPassive for not having to deal with polling the file descriptors
 * and managing the ggzmod object.
 * Right now, only Guru games use GGZPassive.
 */

#include <ggz_dio.h>

/* Callback type for whenever network data is available */
typedef void (*GGZGameFunction) (void);

/* Sets the network callback */
void ggzpassive_sethandler(GGZGameFunction func);
/* Instructs ggzpassive to expect a quantized (dio-style) protocol */
void ggzpassive_enabledio(void);
/* Main loop */
void ggzpassive_start(void);
/* Calling this instructs ggzpassive to exit the main loop */
void ggzpassive_end(void);

/* Network socket for quantized (dio-style) protocols */
extern GGZDataIO *ggz_dio;
/* Network socket for non-quantized (easysock-style) protocols */
extern int ggz_gamefd;

#endif

