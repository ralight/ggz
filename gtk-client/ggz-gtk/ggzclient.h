/*
 * File: ggzclient.h
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: ggzclient.h 10274 2008-07-10 21:38:34Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000-2002 Justin Zaun.
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

#ifndef __GGZCLIENT_H__
#define __GGZCLIENT_H__

#include "ggzcore.h"
#include "support.h"

void INTERNAL ggz_event_init(GGZServer *Server);
void ggz_sensitivity_init(void);
int INTERNAL ggz_connection_query(void);

/* Should be called any time we disconnect from the server. */
void INTERNAL server_disconnect(void);

#endif /* __GGZCLIENT_H__ */
