/* 
 * File: ggzcore-ggz.h
 * Author: GGZ Dev Team
 * Project: ggzcore
 * Date: 01/16/2005
 * Desc: GGZ client<->server network connection (GGZ side)
 * $Id: ggzcore-ggz.h 6866 2005-01-24 01:39:48Z jdorje $
 *
 * This file contains the GGZ-only interface for the ggzcore library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and GGZ client.  The functions provided in this file are intended for use
 * inside ggzmod.
 *
 * Copyright (C) 2005 GGZ Development Team.
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


#ifndef __GGZCORE_GGZ_H__
#define __GGZCORE_GGZ_H__

/* Returns the socket FD, or -1 */
int ggzcore_channel_connect(const char *host, unsigned int port,
			    const char *handle);

#endif /* __GGZCORE_GGZ_H__ */
