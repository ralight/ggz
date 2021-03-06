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

import ggz.common.Perm;
import ggz.common.PermSet;
import ggz.common.PlayerType;

import java.io.IOException;
import java.text.Collator;
import java.util.Comparator;

/*
 * The Player structure is meant to be a node in a list of the players in the
 * current room .
 */
public class Player {
    public static final int NO_RECORD = -1;

    public static final int NO_RATING = 0;

    public static final int NO_RANKING = 0;

    public static final int NO_HIGHSCORE = -1;

    /* Name of player */
    private String name;

    /* Type of player */
    private PlayerType type;

    private PermSet perms;

    /* Pointer to room player is in */
    private Room room;

    /* Server ID of table player is at */
    private int table;

    /* Lag of the player */
    private int lag;

    /* Record of the player (or NO_RECORD). */
    public int wins, losses, ties, forfeits;

    /* Rating of the player (or NO_RATING) */
    private int rating;

    /* Ranking of the player (or NO_RANKING) */
    private int ranking;

    /* Player's highest score (or NO_HIGHSCORE) */
    private int highscore;

    public String get_name() {
        return this.name;
    }

    public int get_rating() {
        return this.rating;
    }

    public int get_ranking() {
        return this.ranking;
    }

    public int get_highscore() {
        return this.highscore;
    }

    public int get_wins() {
        return this.wins;
    }

    public int get_losses() {
        return this.losses;
    }

    public int get_ties() {
        return this.ties;
    }

    public int get_forfeits() {
        return this.forfeits;
    }

    /**
     * Constructor used only by RoomChatPanel to calculate column widths.
     * 
     * @param name
     */
    public Player(String name) {
        this.name = name;
    }

    /*
     * Internal library functions (prototypes in player.h) NOTE:All of these
     * functions assume valid inputs!
     */

    Player(String name, Room room, int table, PlayerType type, PermSet perms,
            int lag) {
        this.wins = NO_RECORD;
        this.losses = NO_RECORD;
        this.forfeits = NO_RECORD;
        this.ties = NO_RECORD;
        this.rating = NO_RATING;
        this.ranking = NO_RANKING;
        this.highscore = NO_HIGHSCORE;
        this.name = name;
        this.room = room;
        this.table = table;
        this.type = type;
        this.perms = perms;
        this.lag = lag;
    }

    void set_stats(int new_wins, int new_losses, int new_ties,
            int new_forfeits, int new_rating, int new_ranking, int new_highscore) {
        this.wins = new_wins;
        this.losses = new_losses;
        this.ties = new_ties;
        this.forfeits = new_forfeits;
        this.rating = new_rating;
        this.ranking = new_ranking;
        this.highscore = new_highscore;
    }

    void set_table(int table) {
        this.table = table;
    }

    void set_lag(int lag) {
        this.lag = lag;
    }

    public PlayerType get_type() {
        return this.type;
    }

    public PermSet get_perms() {
        return this.perms;
    }

    public Room get_room() {
        return this.room;
    }

    public Table get_table() {
        if (this.table == -1)
            return null;

        return this.room.get_table_by_id(this.table);
    }

    public int get_lag() {
        return this.lag;
    }

    public boolean has_perm(Perm perm) {
        return this.perms != null && this.perms.isSet(perm);
    }

    /**
     * Sends a request to the server to set the permission. The permission is
     * not set in this player object directly.
     * 
     * @param perm
     * @param set
     * @throws IOException
     */
    public void set_perm(Perm perm, boolean set) throws IOException {
        Server server = get_room().get_server();
        server.set_perm(get_name(), perm, set);
    }

    /**
     * Sens a series of requests to the server to set the given permissions.
     * 
     * @param perms
     * @throws IOException
     */
    protected void set_perms(PermSet perms) {
        this.perms = perms;
    }

    protected void set_type(PlayerType type) {
        this.type = type;
    }

    public boolean equals(Object o) {
        // Player names are not case sensitive.
        return (o != null) && (o instanceof Player)
                && this.name.equalsIgnoreCase(((Player) o).name);
    }

    public String toString() {
        return get_name();
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.name.hashCode();
    }

    public static final Comparator SORT_BY_NAME = new Comparator() {
        private Collator collator = Collator.getInstance();

        public int compare(Object o1, Object o2) {
            Player p1 = (Player) o1;
            Player p2 = (Player) o2;
            if (p1 == null && p2 == null) {
                return 0;
            } else if (p1 == null) {
                return -1;
            } else if (p2 == null) {
                return 1;
            } else {
                return collator.compare(p1.get_name(), p2.get_name());
            }
        }
    };

    public static final Comparator SORT_BY_TYPE = new Comparator() {

        public int compare(Object o1, Object o2) {
            Player p1 = (Player) o1;
            Player p2 = (Player) o2;
            if (p1 == null && p2 == null) {
                return 0;
            } else if (p1 == null) {
                return -1;
            } else if (p2 == null) {
                return 1;
            } else {
                PlayerType type1 = p1.get_type();
                PlayerType type2 = p2.get_type();
                int result = PlayerType.SORT_BY_PRIVILEGE.compare(type1, type2);
                if (result == 0) {
                    return SORT_BY_NAME.compare(p1, p2);
                }
                return result;
            }
        }
    };

}
