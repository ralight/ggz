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

import ggz.common.NumberList;

/**
 * 
 * @author Helg.Bredow
 * 
 */
public class GameType {
    /* Name of the game */
    private String name;

    /* Version */
    private String version;

    /* Protocol engine */
    private String prot_engine;

    /* Protocol version */
    private String prot_version;

    /* Game description */
    private String desc;

    /* Author */
    private String author;

    /* Homepage for game */
    private String url;

    /* Bitmask of alowable numbers of players */
    private NumberList player_allow_list;

    /* Bitmask of alowable numbers of bots */
    private NumberList bot_allow_list;

    /* Whether spectators are allowed or not */
    private boolean spectators_allowed;

    /* ID of this game on the server */
    private int id;

    /* Array of named bots, in name-class pairs */
    private String[][] named_bots;

    /* Whether peer hostname disclosure is allowed or not */
    private boolean peers_allowed;

    GameType(int id, String name, String version, String prot_engine,
            String prot_version, NumberList player_allow_list,
            NumberList bot_allow_list, boolean spectators_allowed,
            boolean peers_allowed, String desc, String author, String url) {
        this.id = id;
        this.player_allow_list = player_allow_list;
        this.bot_allow_list = bot_allow_list;
        this.spectators_allowed = spectators_allowed;
        this.peers_allowed = peers_allowed;

        this.name = name;
        this.version = version;
        this.prot_engine = prot_engine;
        this.prot_version = prot_version;
        this.desc = desc;
        this.author = author;
        this.url = url;

        this.named_bots = null;
    }

    public void add_namedbot(String botname, String botclass) {
        int size = get_num_namedbots();
        // Grow the array by one.
        String[][] old_named_bots = this.named_bots;
        this.named_bots = new String[size + 1][2];
        if (old_named_bots != null) {
            System.arraycopy(old_named_bots, 0, this.named_bots, 0,
                    this.named_bots.length);
        }
        this.named_bots[size][0] = botname;
        this.named_bots[size][1] = botclass;
    }

    int get_id() {
        return this.id;
    }

    public String get_name() {
        return this.name;
    }

    public String get_prot_engine() {
        return this.prot_engine;
    }

    public String get_prot_version() {
        return this.prot_version;
    }

    public String get_version() {
        return this.version;
    }

    public String get_author() {
        return this.author;
    }

    public String get_url() {
        return this.url;
    }

    public String get_desc() {
        return this.desc;
    }

    /* Return the maximum number of allowed players/bots */
    public int get_min_players() {
        return this.player_allow_list.get_min();
    }

    public int get_max_players() {
        return this.player_allow_list.get_max();
    }

    public int get_max_bots() {
        return this.bot_allow_list.get_max();
    }

    public boolean get_spectators_allowed() {
        return this.spectators_allowed;
    }

    public boolean get_peers_allowed() {
        return this.peers_allowed;
    }

    /* Verify that a paticular number of players/bots is valid */
    public boolean num_players_is_valid(int num) {
        return this.player_allow_list.isset(num);
    }

    public boolean num_bots_is_valid(int num) {
        return num == 0 || this.bot_allow_list.isset(num);
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof GameType) && this.id == ((GameType) o).id;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.id;
    }
    
    public int get_num_namedbots() {
        return named_bots == null ? 0 : named_bots.length;
    }

    public String get_namedbot_name(int num) {
        return this.named_bots[num][0];
    }

    public String get_namedbot_class(int num) {
        return this.named_bots[num][1];
    }
}
