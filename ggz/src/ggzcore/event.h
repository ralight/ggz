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

/* _ggzcore_event_init() - Initialize event system
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_event_init(void);

/* _ggzcore_event_ispending_actual() - Determine if there are any
 *                                     pending GGZ events
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int _ggzcore_event_ispending_actual(void);


/* _ggzcore_event_remove_all_callbacks() - Remove all callbacks from an event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_event_remove_all_callbacks(const GGZEventID id);


/* ggzcore_event_destroy() - Cleanup event system
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_event_destroy(void);


#endif /* __EVENT_H__ */
