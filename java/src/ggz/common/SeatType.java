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
 * @brief Seat status values.
 * 
 * Each "seat" at a table of a GGZ game can have one of these values. They are
 * used by the GGZ client, GGZ server, and game servers; their use in game
 * clients is completely optional.
 */
public class SeatType {

	public static final ArrayList values = new ArrayList(7);

	/** This seat does not exist. */
	public static final SeatType GGZ_SEAT_NONE = new SeatType();

	/** The seat is open (unoccupied). */
	public static final SeatType GGZ_SEAT_OPEN = new SeatType();

	/** The seat has a bot (AI) in it. */
	public static final SeatType GGZ_SEAT_BOT = new SeatType();

	/** The seat has a regular player in it. */
	public static final SeatType GGZ_SEAT_PLAYER = new SeatType();

	/** The seat is reserved for a player. */
	public static final SeatType GGZ_SEAT_RESERVED = new SeatType();

	/** The seat is abandoned by a player. */
	public static final SeatType GGZ_SEAT_ABANDONED = new SeatType();

	private static final String OPEN_SEAT_NAME = "open";

	private static final String BOT_SEAT_NAME = "bot";

	private static final String PLAYER_SEAT_NAME = "player";

	private static final String RESERVED_SEAT_NAME = "reserved";

	private static final String ABANDONED_SEAT_NAME = "abandoned";

	private static final String NONE_SEAT_NAME = "none";

	private SeatType() {
		values.add(this);
	}

	public String toString() {
		if (this == GGZ_SEAT_OPEN) {
			return OPEN_SEAT_NAME;
		} else if (this == GGZ_SEAT_BOT) {
			return BOT_SEAT_NAME;
		} else if (this == GGZ_SEAT_RESERVED) {
			return RESERVED_SEAT_NAME;
		} else if (this == GGZ_SEAT_PLAYER) {
			return PLAYER_SEAT_NAME;
		} else if (this == GGZ_SEAT_ABANDONED) {
			return ABANDONED_SEAT_NAME;
		} else if (this == GGZ_SEAT_NONE) {
			return NONE_SEAT_NAME;
		}

		throw new RuntimeException("Unrecognised SeatType: " + this);
	}

	public static SeatType valueOf(String type_str) {
		/*
		 * If it doesn't match _exactly_ we return GGZ_SEAT_NONE. This is bad
		 * for, say, user input, but perfectly acceptable as an inverse to
		 * ggz_seattype_to_string().
		 */
		if (type_str == null)
			return GGZ_SEAT_NONE;
		else if (OPEN_SEAT_NAME.equals(type_str))
			return GGZ_SEAT_OPEN;
		else if (BOT_SEAT_NAME.equals(type_str))
			return GGZ_SEAT_BOT;
		else if (RESERVED_SEAT_NAME.equals(type_str))
			return GGZ_SEAT_RESERVED;
		else if (PLAYER_SEAT_NAME.equals(type_str))
			return GGZ_SEAT_PLAYER;
		else if (ABANDONED_SEAT_NAME.equals(type_str))
			return GGZ_SEAT_ABANDONED;

		return GGZ_SEAT_NONE;
	}

	public static SeatType valueOf(int ordinal) {
		return (SeatType) values.get(ordinal);
	}
    
    public int ordinal() {
        return values.indexOf(this);
    }
}
