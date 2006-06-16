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
package ggz.client.core;

import ggz.common.SeatType;
import ggz.common.TableState;

import java.io.IOException;
import java.util.logging.Logger;

public class Table {
    private static final Logger log = Logger.getLogger(Table.class.getName());

    /* Pointer to room this table resides in */
    private Room room;

    /* Server ID of table */
    private int id;

    /* Game Type */
    private GameType gametype;

    /* Table description */
    private String desc;

    /* Table state */
    private TableState state;

    /* Total seats */
    // TODO consider using seats.length instead.
    private int num_seats;

    /* Seats */
    private TableSeat[] seats;

    /*
     * Spectator seats - "type" is unused; player name is NULL for empty seat.
     */
    private TableSeat[] spectator_seats;

    /*
     * Publicly exported functions
     */

    /**
     * Used for finding tables in lists.
     * 
     * @see equals()
     */
    Table(int id) {
        this.id = id;
    }

    public Table(GameType gametype, String desc, int num_seats) {
        this(gametype, desc, num_seats, TableState.GGZ_TABLE_CREATED, -1);
        if (gametype == null) {
            throw new IllegalArgumentException("gametype cannot be null");
        }
    }

    Table(GameType gametype, String desc, int num_seats, TableState state,
            int id) {
        /* A NULL desc is allowed. */
        this.room = null;
        this.gametype = gametype;
        this.id = id;
        this.state = state;
        this.desc = desc;

        this.num_seats = num_seats;
        log.fine("Allocating " + num_seats + " seats");
        if (num_seats > 0) {
            this.seats = new TableSeat[num_seats];
        }

        for (int i = 0; i < num_seats; i++) {
            this.seats[i] = new TableSeat(i, SeatType.GGZ_SEAT_NONE, null);
        }

        /* Allocated on demand later */
        this.spectator_seats = null;
    }

    public void set_seat(int index, SeatType type, String name)
            throws IOException {
        Server server;
        Net net;
        TableSeat seat = new TableSeat(index, type, name);

        log.fine("User changing seats... on " + this);

        /* Check table and seat number. */
        if (index >= this.num_seats)
            throw new IllegalArgumentException(
                    "index is greater than number of seats");

        /* GGZ clients should only ever set seats to OPEN, BOT, or RESERVED. */
        if (type != SeatType.GGZ_SEAT_OPEN && type != SeatType.GGZ_SEAT_BOT
                && type != SeatType.GGZ_SEAT_RESERVED)
            throw new IllegalArgumentException(
                    "GGZ clients should only ever set seats to OPEN, BOT, or RESERVED");

        /* If we set a seat to RESERVED, we need a reservation name. */
        if (type == SeatType.GGZ_SEAT_RESERVED && name == null)
            throw new IllegalArgumentException(
                    "if type is GGZ_SEAT_RESERVED then a name is required");

        /*
         * If the table is newly created and not involved in a game yet, users
         * may change seats all they want
         */
        if (this.state == TableState.GGZ_TABLE_CREATED)
            set_seat(seat);
        else {
            /* Otherwise we have to do it through the server */
            if (this.room == null)
                throw new IllegalStateException("table does not have a room");

            if ((server = room.get_server()) == null)
                throw new IllegalStateException("room does not have a server");

            if ((net = server.get_net()) == null)
                throw new IllegalStateException("server does not have a net");

            net.send_table_seat_update(this, seat);
        }
    }

    void set_desc(String desc) throws IOException {
        Server server;
        Net net;

        log.fine("User changing desc... on " + this);

        /*
         * If the table is newly created and not involved in a game yet, users
         * may change desc all they want
         */
        if (this.state == TableState.GGZ_TABLE_CREATED) {
            log.fine("Table " + this.id + "new desc " + desc);

            this.desc = desc;

            /* If we're in a room, notify it of a table event */
            if (this.room != null)
                this.room.table_event(RoomEvent.GGZ_TABLE_UPDATE, this);
        } else {
            /* Otherwise we have to do it through the server */
            if (this.room == null)
                throw new IllegalStateException("table is not in a room");

            if ((server = this.room.get_server()) == null)
                throw new IllegalStateException("room does not have a server");

            if ((net = server.get_net()) == null)
                throw new IllegalStateException("server does not have a net");

            net.send_table_desc_update(this, desc);
        }
    }

    void remove_player(String name) {
        if (name != null) {
            for (int i = 0; i < this.num_seats; i++)
                if (this.seats[i].name != null
                        && this.seats[i].name.equals(name)) {
                    TableSeat seat = new TableSeat(i, SeatType.GGZ_SEAT_OPEN,
                            null);

                    set_seat(seat);
                }
        }
    }

    void set_room(Room room) {
        this.room = room;
    }

    void set_id(int id) {
        this.id = id;
    }

    void set_state(TableState state) {
        log.fine("Table " + this.id + " new state " + state);
        this.state = state;

        /* If we're in a room, notify it of a table event */
        if (this.room != null)
            this.room.table_event(RoomEvent.GGZ_TABLE_UPDATE, this);
    }

    void set_seat(TableSeat seat) {
        /* Set up the new seat. */
        TableSeat oldseat;
        Server server;
        Game game;

        /* Sanity check */
        if (seat.index >= this.num_seats) {
            log.warning("Attempt to set seat " + seat.index
                    + " on table with only " + this.num_seats + " seats");
        }

        oldseat = this.seats[seat.index];
        this.seats[seat.index] = seat;

        /* Check for specific seat changes */
        if (seat.type == SeatType.GGZ_SEAT_PLAYER) {
            log.fine(seat.name + " joining seat " + seat.index + " at table "
                    + this.id);
            if (this.room != null)
                this.room.player_set_table(seat.name, this.id);
        } else if (oldseat.type == SeatType.GGZ_SEAT_PLAYER) {
            log.fine(oldseat.name + " leaving seat " + oldseat.index
                    + " at table " + this.id);
            if (this.room != null)
                this.room.player_set_table(oldseat.name, -1);
        } else {
            if (this.room != null)
                this.room.table_event(RoomEvent.GGZ_TABLE_UPDATE, this);
        }

        /* If this is our table, alert the game module. */
        if (this.room != null && ((server = this.room.get_server()) != null)
                && ((game = server.get_cur_game()) != null)
                && this.room.get_id() == game.get_room_id()) {
            String me = server.get_handle();
            int game_table = game.get_table_id();

            if (this.id == game_table)
                game.set_seat(seat);
            if (seat.type == SeatType.GGZ_SEAT_PLAYER && me.equals(seat.name)) {
                game.set_player(false, seat.index);
                if (game_table < 0) {
                    game.set_table(game.get_room_id(), this.id);
                }
            }
        }
    }

    void set_spectator_seat(TableSeat seat) {
        String oldSeatName;
        Server server;
        Game game;

        // Check if our array is big enough to hold the new seat.
        if (seat.index >= get_num_spectator_seats()) {
            int oldLength = get_num_spectator_seats();
            int newLength = oldLength;

            /*
             * Grow the array geometrically to keep a constant ammortized
             * overhead.
             */
            while (seat.index >= newLength) {
                newLength = newLength > 0 ? newLength * 2 : 1;
            }

            log.fine("Increasing number of spectator seats to " + newLength
                    + ".");

            TableSeat[] oldArray = this.spectator_seats;
            this.spectator_seats = new TableSeat[newLength];
            if (oldArray != null) {
                System.arraycopy(oldArray, 0, this.spectator_seats, 0,
                        oldArray.length);
            }

            for (int i = oldLength; i < newLength; i++) {
                this.spectator_seats[i] = new TableSeat(i,
                        SeatType.GGZ_SEAT_NONE, null);
            }
        }

        oldSeatName = this.spectator_seats[seat.index].name;
        this.spectator_seats[seat.index] = seat;

        /* Check for specific seat changes */
        if (seat.name != null) {
            log.fine(seat.name + " spectating seat " + seat.index
                    + " at table " + this.id);
            if (this.room != null)
                this.room.player_set_table(seat.name, this.id);
        }

        if (oldSeatName != null) {
            log.fine(oldSeatName + " stopped spectating seat at table "
                    + this.id);
            if (this.room != null) {
                this.room.player_set_table(oldSeatName, -1);
            }
        }

        /* If this is our table, alert the game module. */
        if (this.room != null && ((server = this.room.get_server()) != null)
                && ((game = server.get_cur_game()) != null)
                && this.room.get_id() == game.get_room_id()) {
            String me = server.get_handle();
            int game_table = game.get_table_id();

            if (this.id == game_table) {
                game.set_spectator_seat(seat);
            }

            if (me.equals(seat.name)) {
                game.set_player(true, seat.index);
                if (game_table < 0) {
                    game.set_table(game.get_room_id(), this.id);
                }
            }
        }
    }

    public Room get_room() {
        return this.room;
    }

    public int get_id() {
        return this.id;
    }

    public GameType get_type() {
        return this.gametype;
    }

    public String get_desc() {
        return this.desc;
    }

    public TableState get_state() {
        return this.state;
    }

    public int get_num_seats() {
        return this.num_seats;
    }

    public int get_seat_count(SeatType type) {
        int count = 0;

        for (int i = 0; i < this.num_seats; i++)
            if (this.seats[i].type == type)
                count++;

        return count;
    }

    public TableSeat get_nth_seat(int num) {
        return this.seats[num];
    }

    public String get_nth_player_name(int num) {
        if (num < this.num_seats) {
            return this.seats[num].name;
        }
        return null;
    }

    public SeatType get_nth_player_type(int num) {
        if (num < this.num_seats) {
            return this.seats[num].type;
        }
        return SeatType.GGZ_SEAT_NONE;
    }

    public int get_num_spectator_seats() {
        return this.spectator_seats == null ? 0 : this.spectator_seats.length;
    }

    public TableSeat get_nth_spectator_seat(int num) {
        return this.spectator_seats[num];
    }

    public String get_nth_spectator_name(int num) {
        if (num >= get_num_spectator_seats())
            return null;
        return this.spectator_seats[num].name;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof Table) && this.id == ((Table) o).id;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.id;
    }
}
