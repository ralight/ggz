/*
 * File: event.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * This is the code for handling high-level client events
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#ifndef __EVENT_H__
#define __EVENT_H__

/* ggzcore_event_init() - Initialize event system
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_event_init(void);


/* ggzcore_event_destroy() - Cleanup event system
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_event_destroy(void);


#endif /* __EVENT_H__ */
