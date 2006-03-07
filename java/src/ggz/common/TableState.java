/*
 * Copyright (C) 2006  Helg Bredow
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package ggz.common;

import java.util.ArrayList;

/**
 * Table state values.
 * 
 * GGZ tables take one of the following states. They are used by the GGZ client
 * and GGZ server. This is *not* necessarily the same as the game state kept by
 * libggzdmod
 */
public class TableState {

    private static ArrayList values = new ArrayList();

    /** There is some error with the table. */
	public static final TableState GGZ_TABLE_ERROR = new TableState(); // = -1,

	/** Initial created state for the table. */
	public static final TableState GGZ_TABLE_CREATED = new TableState(); // = 0,

	/** Waiting for enough players to join before playing. */
	public static final TableState GGZ_TABLE_WAITING = new TableState(); // = 1,

	/** Playing a game. */
	public static final TableState GGZ_TABLE_PLAYING = new TableState(); // = 2,

	/** The game session is finished and the table will soon exit. */
	public static final TableState GGZ_TABLE_DONE = new TableState(); // = 3
    
    private TableState() {
    	values.add(this);
    }

	public static TableState valueOf(int i) {
		if (i == -1) {
			return GGZ_TABLE_ERROR;
		}
		return (TableState) values.get(i + 1);
	}
}
