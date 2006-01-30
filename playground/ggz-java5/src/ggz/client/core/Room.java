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

import ggz.common.ChatType;
import ggz.common.ClientReqError;
import ggz.common.LeaveType;
import ggz.common.PlayerType;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Logger;

import javax.swing.event.EventListenerList;

/*
 * The GGZRoom struct manages information about a particular room
 */
public class Room {
    private static final Logger log = Logger.getLogger(Room.class.getName());

    /* Array of GGZRoom messages */
    static final String[] events = new String[] { "GGZ_PLAYER_LIST",
            "GGZ_TABLE_LIST", "GGZ_CHATevent", "GGZ_ROOM_ENTER",
            "GGZ_ROOM_LEAVE", "GGZ_TABLE_UPDATE", "GGZ_TABLE_LAUNCHED",
            "GGZ_TABLE_LAUNCH_FAIL", "GGZ_TABLE_JOINED", "GGZ_TABLE_JOIN_FAIL",
            "GGZ_TABLE_LEFT", "GGZ_TABLE_LEAVE_FAIL", "GGZ_PLAYER_LAG",
            "GGZ_PLAYER_STATS", "GGZ_PLAYER_COUNT" };

    /* Server which this room is on */
    private Server server;

    /* Room ID on the server */
    private int id;

    /* Name of room */
    private String name;

    /* Supported game type (ID on server) */
    private int game_type_id;

    /* Room description */
    private String desc;

    /* Number of players in list room (current room only) */
    private int num_players;

    /* List of players in the room (current room only) */
    private List<Player> players;

    /* Number of players we suspect are in the room */
    private int player_count;

    /* List of tables in the room (current room only) */
    private List<Table> tables;

    /* Room events */
    private HookList event_hooks;

    public Player get_player_by_name(String player_name) {
        int entry;
        Player player, found = null;

        if (this.players != null) {
            player = new Player(player_name, null, -1, PlayerType.values()[0],
                    0);
            entry = this.players.indexOf(player);

            if (entry > -1)
                found = this.players.get(entry);
        }

        return found;
    }

    public Server get_server() {
        return this.server;
    }

    public int get_id() {
        return this.id;
    }

    public String get_name() {
        return this.name;
    }

    public String get_desc() {
        return this.desc;
    }

    public GameType get_gametype() {
        return this.server.get_type_by_id(this.game_type_id);
    }

    public int get_num_players() {
        if (this.server.get_cur_room() == this) {
            return this.num_players;
        }
        return this.player_count;
    }

    public int get_num_tables() {
        return this.tables == null ? 0 : this.tables.size();
    }

    public Player get_nth_player(int num) {
        if (num < this.num_players) {
            return this.players.get(num);
        }
        return null;
    }

    public Table get_nth_table(int num) {
        if (num < get_num_tables()) {
            return this.tables.get(num);
        }
        return null;
    }

    public Table get_table_by_id(int table_id) {
        Table found = null;

        if (this.tables != null) {
            Table search_table = new Table(table_id);
            int entry = this.tables.indexOf(search_table);

            if (entry > -1)
                found = this.tables.get(entry);

        }

        return found;
    }

    /* Functions for manipulating hooks to GGZRoom events */
    // int addevent_hook( RoomEvent event,
    // const GGZHookFunc func)
    // {
    // if (room && func && event_is_valid(event))
    // return _addevent_hook_full(room, event, func,
    // null);
    // else
    // return -1;
    // }
    //
    //
    // int addevent_hook_full(
    // RoomEvent event,
    // const GGZHookFunc func,
    // const void *data)
    // {
    // if (room && func && event_is_valid(event))
    // return _addevent_hook_full(room, event, func,
    // data);
    // else
    // return -1;
    // }
    //
    //
    // int removeevent_hook(
    // RoomEvent event,
    // const GGZHookFunc func)
    // {
    // if (room && func && event_is_valid(event))
    // return _removeevent_hook(room, event, func);
    // else
    // return -1;
    // }
    //
    //
    // int removeevent_hook_id(
    // RoomEvent event,
    // int hook_id)
    // {
    // if (room && event_is_valid(event))
    // return _removeevent_hook_id(room, event,
    // hook_id);
    // else
    // return -1;
    // }
    public void list_players() throws IOException {
        if (this.server != null) {
            Net net = this.server.get_net();
            if (net != null) {
                net.send_list_players();
            }
        }
    }

    public void list_tables() throws IOException {
        if (this.server != null) {
            Net net = this.server.get_net();
            if (net != null) {
                net.send_list_tables();
            }
        }
    }

    public void chat(ChatType type, String player, String msg)
            throws IOException {
        if (this.server != null) {
            /* FIXME: check validty of args */
            Net net;

            net = this.server.get_net();

            if (msg != null && msg.indexOf('\n') > -1) {
                /*
                 * If the chat includes multiple lines, then we send each line
                 * as a separate chat item. The implementation of this is a
                 * little inefficient, but I can't see a better way to do it
                 * (cleanly). Also it is possible that we should treat \r and
                 * \r\n as breaks instead of just \n.
                 * 
                 * Note that if you send a string like "\n\n", this code will
                 * simply send the empty string twice...and the server will
                 * ignore both of them. So we might also want to check for empty
                 * strings and just not send them.
                 * 
                 * As far as I can tell, this implementation is functionally
                 * equivalent to common IRC behavior.
                 */
                // int len = msg.length();
                // char text[len + 1];
                // char *this = text, *newline;
                //
                // strncpy(text, msg, len);
                // text[len] = '\0';
                //
                // while ((newline = strchr(this, '\n'))) {
                // *newline = '\0';
                // if (_ggzcore_net_send_chat(net, type,
                // player, this) < 0) {
                // return -1;
                // }
                // this = newline + 1;
                // }
                // return _ggzcore_net_send_chat(net, type, player, this);
                StringTokenizer line_splitter = new StringTokenizer(msg, "\n",
                        false);
                while (line_splitter.hasMoreTokens()) {
                    net.send_chat(type, player, line_splitter.nextToken());
                }
            } else {
                net.send_chat(type, player, msg);
            }
        }
        // else
        // return -1;
    }

    public void launch_table(Table table) throws IOException {
        if (this.server != null && table != null) {
            Net net;
            Game game = this.server.get_cur_game();

            /*
             * Make sure we're actually in a room.
             */
            if (this.server.get_state() != StateID.GGZ_STATE_IN_ROOM) {
                throw new IllegalStateException("Server is not in state: "
                        + StateID.GGZ_STATE_IN_ROOM);
            }
            if (game == null) {
                throw new IllegalStateException(
                        "Cannot launch table until the game has been launched.");
            }
            if (this.server.get_cur_room() != this) {
                throw new IllegalStateException(
                        "Can only launch table for current room.");
            }

            net = this.server.get_net();
            net.send_table_launch(table);
            game.set_player(false, -1);
            this.server.set_table_launching();
        } else {
            throw new IllegalStateException(
                    "server is null or table parameter is null");
        }
    }

    public void join_table(int num, boolean spectator) throws IOException {
        if (this.server == null || this.server.get_cur_game() == null) {
            throw new IllegalStateException("server or server.cur_game is null");
        }

        Net net;
        Game game = this.server.get_cur_game();
        Room cur_room = this.server.get_cur_room();

        /*
         * Make sure we're actually in this room, and a game object has been
         * created, and that we know the table exists. FIXME: make sure the game
         * isn't already launched...
         */
        if (this.server.get_state() != StateID.GGZ_STATE_IN_ROOM
                || cur_room == null || this.id != cur_room.id || game == null
                || get_table_by_id(num) == null)
            throw new IllegalStateException();

        net = this.server.get_net();
        net.send_table_join(num, spectator);

        game.set_table(this.id, num);
        game.set_player(spectator, -1);

        this.server.set_table_joining();
    }

    public void leave_table(boolean force) throws IOException {
        if (this.server != null)
            _leave_table(force);
        else
            throw new IllegalStateException();
    }

    /*
     * Internal library functions (prototypes in room.h) NOTE:All of these
     * functions assume valid inputs!
     */

    /* Create a new room object for a given server */
    Room(Server server, int id, String name, int game, String desc,
            int player_count) {
        this.server = server;
        this.id = id;
        this.game_type_id = game;
        this.name = name;
        this.desc = desc;
        this.player_count = player_count;

        /* FIXME: create player list? */
        /* FIXME: create table list? */

        /* Setup event hook list */
        // for (i = 0; i < _ggzcore_numevents; i++)
        // this.event_hooks[i] = _ggzcore_hook_list_init(i);
        this.event_hooks = new HookList();
    }

    void set_player_list(int count, List<Player> list) {
        boolean count_changed = (count != this.num_players);

        /* Get rid of old list */
        // ggz_list_free(this.players);
        this.num_players = count;
        this.player_count = count;
        this.players = list;

        event(RoomEvent.GGZ_PLAYER_LIST, this.id);
        if (count_changed) {
            server.queue_players_changed();
        }
    }

    public void set_players(int players) {
        if (this.player_count == players) {
            return;
        }
        this.player_count = players;
        if (this.player_count < 0) {
            /* Sanity check. */
            this.player_count = 0;
        }
        event(RoomEvent.GGZ_PLAYER_COUNT, this.id);
        server.queue_players_changed();
    }

    /* Room-player functions. */

    void set_player_lag(String name, int lag) {
        /* FIXME: This should be sending a player "class-based" event */
        Player player;

        log.fine("Setting lag to " + lag + " for " + name);

        player = get_player_by_name(name);
        if (player != null) { /* make sure they're still in room */
            player.set_lag(lag);
            event(RoomEvent.GGZ_PLAYER_LAG, name);
        }
    }

    void set_player_stats(Player pdata) {
        /* FIXME: This should be sending a player "class-based" event */
        Player player;

        log.fine("Setting stats for " + pdata.get_name());

        player = get_player_by_name(pdata.get_name());

        /* make sure they're still in room */
        if (player == null) {
            return;
        }

        player.init_stats(pdata.get_wins(), pdata.get_losses(), pdata
                .get_ties(), pdata.get_forfeits(), pdata.get_rating(), pdata
                .get_ranking(), pdata.get_highscore());
        event(RoomEvent.GGZ_PLAYER_STATS, this.name);
    }

    void set_table_list(List<Table> list) {
        this.tables = list;

        /* Sanity check: make sure these tables point to us */
        for (Iterator<Table> iter = list.iterator(); iter.hasNext();) {
            Table table = iter.next();
            table.set_room(this);
        }

        event(RoomEvent.GGZ_TABLE_LIST, null);
    }

    void set_monitor(boolean monitor) {
        /* If turning off monitoring, clear lists */
        if (!monitor) {
            this.num_players = 0;
            this.players = null;
            this.tables = null;
        }
    }

    void add_player(Player pdata, int from_room) {
        Player player;
        RoomChangeEventData data = new RoomChangeEventData();

        log.fine("Adding player " + pdata.get_name());

        /* Create the list if it doesn't exist yet */
        if (this.players == null)
            this.players = new ArrayList<Player>();

        /* Default new people in room to no table (-1) */
        player = new Player(pdata.get_name(), pdata.get_room(), -1, pdata
                .get_type(), pdata.get_lag());
        player.init_stats(pdata.get_wins(), pdata.get_losses(), pdata
                .get_ties(), pdata.get_forfeits(), pdata.get_rating(), pdata
                .get_ranking(), pdata.get_highscore());

        this.players.add(player);
        this.num_players++;
        this.player_count = this.num_players;

        data.player_name = pdata.get_name();
        data.from_room = from_room;
        data.to_room = this.id;
        event(RoomEvent.GGZ_ROOM_ENTER, data);

        Room from;
        if ((from = this.server.get_room_by_id(from_room)) != null) {
            from.set_players(from.player_count - 1);
        }
        server.queue_players_changed();
    }

    void remove_player(String player_name, int to_room) {
        int entry;
        RoomChangeEventData data = new RoomChangeEventData();

        log.fine("Removing player " + player_name);

        /* Only try to delete if the list exists */
        if (this.players != null) {
            Player player = new Player(player_name, null, -1, PlayerType
                    .values()[0], 0);
            entry = this.players.indexOf(player);
            if (entry > -1) {
                this.players.remove(entry);
                this.num_players--;
                this.player_count = this.num_players;

                data.player_name = player_name;
                data.from_room = this.id;
                data.to_room = to_room;

                event(RoomEvent.GGZ_ROOM_LEAVE, data);
                server.queue_players_changed();
            }
        }

        Room to;
        if ((to = this.server.get_room_by_id(to_room)) != null) {
            to.set_players(to.player_count + 1);
        }
    }

    void add_table(Table table) {
        log.fine("Adding table " + table.get_id());

        /* Set table to point to this room */
        table.set_room(this);

        /* Create the list if it doesn't exist yet */
        if (this.tables == null) {
            this.tables = new ArrayList<Table>();
        }

        this.tables.add(table);
        event(RoomEvent.GGZ_TABLE_UPDATE, null);
    }

    void remove_table(int table_id) {
        log.fine("Deleting table: " + table_id);

        /* Only try to delete if the list exists */
        if (this.tables != null) {
            int entry;
            Table search_table = new Table(table_id);

            entry = this.tables.indexOf(search_table);
            if (entry > -1) {
                this.tables.remove(entry);
                event(RoomEvent.GGZ_TABLE_UPDATE, null);
            }

        }
    }

    void player_set_table(String player_name, int table) {
        Player player;

        log.fine(player_name + " table is now " + table);

        if (this.players != null) {
            /* make sure they're still in room */
            if ((player = get_player_by_name(player_name)) != null)
                player.set_table(table);
        }

        event(RoomEvent.GGZ_TABLE_UPDATE, null);
    }

    void add_chat(ChatType type, String player_name, String msg) {
        ChatEventData data = new ChatEventData(type, player_name, msg);

        log.fine("Chat (" + type.toString() + ") from " + player_name);

        event(RoomEvent.GGZ_CHAT_EVENT, data);
        if (type == ChatType.GGZ_CHAT_TABLE) {
            Game game = this.server.get_cur_game();

            game.inform_chat(player_name, msg);
        }
    }

    void set_table_launch_status(ClientReqError status) {
        this.server.set_table_launch_status(status);

        if (status == ClientReqError.E_OK) {
            event(RoomEvent.GGZ_TABLE_LAUNCHED, null);
        } else {
            ErrorEventData error = new ErrorEventData(status);

            switch (status) {
            case E_BAD_OPTIONS:
                error.message = "Bad option";
                break;
            case E_ROOM_FULL:
                error.message = "The room has reached its table limit.";
                break;
            case E_LAUNCH_FAIL:
                error.message = "Launch failed on server";
                break;
            case E_NOT_IN_ROOM:
                error.message = "Not in a room";
                break;
            case E_AT_TABLE:
                error.message = "Already at a table";
                break;
            case E_IN_TRANSIT:
                error.message = "Already joining/leaving a table";
                break;
            case E_NO_PERMISSION:
                error.message = "Insufficient permissions";
                break;
            default:
                error.message = "Unknown launch failure";
                break;
            }
            event(RoomEvent.GGZ_TABLE_LAUNCH_FAIL, error);
        }
    }

    /* OK, we've joined a table. Deal with it. */
    void set_table_join(int table_index) throws IOException {
        log.fine("Player joined table " + table_index + ".");
        this.server.set_table_join_status(ClientReqError.E_OK);
        event(RoomEvent.GGZ_TABLE_JOINED, table_index);

        if (this.server.get_cur_game() == null) {
            /*
             * GGZd thinks we're at a table but we know there's no game client
             * running. Probably the game client exited during the connection
             * process. So, tell ggzd that we've left.
             */
            leave_table(true);
        }
    }

    void set_table_join_status(ClientReqError status) {

        if (status != ClientReqError.E_OK)
            this.server.set_table_join_status(status);

        switch (status) {
        case E_OK:
            /*
             * Do nothing if successful. The join itself will be handled
             * separately. See set_table_join.
             */
            break;

        case E_NOT_IN_ROOM:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "Not in a room");
            break;

        case E_AT_TABLE:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "Already at a table");
            break;

        case E_IN_TRANSIT:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL,
                    "Already joining/leaving a table");
            break;

        case E_BAD_OPTIONS:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "Bad option");
            break;

        case E_NO_TABLE:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "No such table");
            break;

        case E_TABLE_FULL:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "The table is full "
                    + "(or has reserved seats)");
            break;

        case E_NO_PERMISSION:
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, "You don't have enough "
                    + "permissions to join this table.");
            break;

        default:
            String buf = "Unknown join failure (status " + status + ")";
            event(RoomEvent.GGZ_TABLE_JOIN_FAIL, buf);
            break;

        }
    }

    void set_table_leave(LeaveType reason, String player) {
        TableLeaveEventData event_data = new TableLeaveEventData(reason, player);
        log.fine("Player left table: " + reason.toString() + " (" + player
                + ").");
        this.server.set_table_leave_status(ClientReqError.E_OK);
        event(RoomEvent.GGZ_TABLE_LEFT, event_data);
    }

    void set_table_leave_status(ClientReqError status) {
        if (status != ClientReqError.E_OK)
            this.server.set_table_leave_status(status);

        switch (status) {
        case E_OK:
            /*
             * Do nothing if successful. The join itself will be handled
             * separately. See _set_table_leave.
             */
            break;

        case E_NOT_IN_ROOM:
            event(RoomEvent.GGZ_TABLE_LEAVE_FAIL, "Not at a table");
            break;

        case E_NO_TABLE:
            event(RoomEvent.GGZ_TABLE_LEAVE_FAIL, "No such table");
            break;

        case E_LEAVE_FORBIDDEN:
            event(RoomEvent.GGZ_TABLE_LEAVE_FAIL,
                    "Cannot leave games of this type");
            break;

        default:
            String buf = "Unknown leave failure (status " + status + ")";
            event(RoomEvent.GGZ_TABLE_LEAVE_FAIL, buf);
            break;
        }
    }

    void table_event(RoomEvent event, Object data) {
        event(event, data);
    }

    void _leave_table(boolean force) throws IOException {
        Net net;
        Game game = this.server.get_cur_game();
        boolean spectating;

        /* Game may be null if the game client has already exited. */

        /*
         * Make sure we're at a table.
         */
        if (this.server.get_state() != StateID.GGZ_STATE_AT_TABLE) {
            throw new IllegalStateException("State is not currently "
                    + StateID.GGZ_STATE_AT_TABLE);
        }
        if (this.server.get_cur_room() != this) {
            throw new IllegalStateException(
                    "Not at table in current room. How did that happen?");
        }

        net = this.server.get_net();
        if (game != null) {
            spectating = game.is_spectator();
        } else {
            spectating = false;
        }
        net.send_table_leave(force, spectating);

        this.server.set_table_leaving();
    }

    /* Functions for attaching hooks to GGZRoom events */
    // int _addevent_hook_full(
    // RoomEvent event,
    // const GGZHookFunc func,
    // const void *data)
    // {
    // return _ggzcore_hook_add_full(this.event_hooks[event], func,
    // data);
    // }
    /* Functions for removing hooks from GGZRoom events */
    // int _removeevent_hook(
    // RoomEvent event,
    // const GGZHookFunc func)
    // {
    // return _ggzcore_hook_remove(this.event_hooks[event], func);
    // }
    //
    //
    // int _removeevent_hook_id(
    // RoomEvent event,
    // int hook_id)
    // {
    // return _ggzcore_hook_remove_id(this.event_hooks[event], hook_id);
    // }
    /**
     * This is different to the C code since Java has better support for event
     * listeners.
     * 
     * @param l
     */
    public void add_event_hook(RoomListener l) {
        event_hooks.addRoomListener(l);
    }

    /**
     * This is different to the C code since Java has better support for event
     * listeners.
     * 
     * @param l
     */
    public void remove_event_hook(RoomListener l) {
        event_hooks.removeRoomListener(l);
    }

    /* Return 0 if equal and -1 otherwise */
    // int _compare(void *p, void *q)
    // {
    // if (((GGZRoom *) p)->id == ((GGZRoom *) q)->id)
    // return 0;
    //
    // return -1;
    // }
    public boolean equals(Object o) {
        return (o != null) && (o instanceof Room) && this.id == ((Room) o).id;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.id;
    }

    /* Create a copy of a room object */
    // void *_copy(void *p)
    // {
    // GGZRoom *new, *src = p;
    //
    // new = _new();
    // _init(new, src->server, src->id, src->name, src->game,
    // src->desc, src->player_count);
    //
    // return (void *)new;
    // }
    /* Static functions internal to this file */

    private void event(RoomEvent event_id, Object data) {
        switch (event_id) {
        case GGZ_PLAYER_LIST:
            event_hooks.fire_player_list(((Integer) data).intValue());
            break;
        case GGZ_TABLE_LIST:
            event_hooks.fire_table_list();
            break;
        case GGZ_CHAT_EVENT:
            event_hooks.fire_chat_event((ChatEventData) data);
            break;
        case GGZ_ROOM_ENTER:
            event_hooks.fire_room_enter((RoomChangeEventData) data);
            break;
        case GGZ_ROOM_LEAVE:
            event_hooks.fire_room_leave((RoomChangeEventData) data);
            break;
        case GGZ_TABLE_UPDATE:
            event_hooks.fire_table_update();
            break;
        case GGZ_TABLE_LAUNCHED:
            event_hooks.fire_table_launched();
            break;
        case GGZ_TABLE_LAUNCH_FAIL:
            event_hooks.fire_table_launch_fail((ErrorEventData) data);
            break;
        case GGZ_TABLE_JOINED:
            event_hooks.fire_table_joined(((Integer) data).intValue());
            break;
        case GGZ_TABLE_JOIN_FAIL:
            event_hooks.fire_table_join_fail((String) data);
            break;
        case GGZ_TABLE_LEFT:
            event_hooks.fire_table_left((TableLeaveEventData) data);
            break;
        case GGZ_TABLE_LEAVE_FAIL:
            event_hooks.fire_table_leave_fail((String) data);
            break;
        case GGZ_PLAYER_LAG:
            event_hooks.fire_player_lag((String) data);
            break;
        case GGZ_PLAYER_STATS:
            event_hooks.fire_player_stats((String) data);
            break;
        case GGZ_PLAYER_COUNT:
            event_hooks.fire_player_count(((Integer) data).intValue());
            break;
        default:
            throw new IllegalArgumentException("Unhandled RoomEvent: "
                    + event_id);
        }
    }

    private class HookList {
        private EventListenerList listeners = new EventListenerList();

        public void addRoomListener(RoomListener l) {
            // Remove it first to prevent duplicates.
            listeners.remove(RoomListener.class, l);
            listeners.add(RoomListener.class, l);
        }

        public void removeRoomListener(RoomListener l) {
            listeners.remove(RoomListener.class, l);
        }

        public void fire_chat_event(ChatEventData data) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.chat_event(data);
            }
        }

        public void fire_player_count(int room_id) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.player_count(room_id);
            }
        }

        public void fire_player_lag(String player) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.player_lag(player);
            }
        }

        public void fire_player_list(int room_id) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.player_list(room_id);
            }
        }

        public void fire_player_stats(String player) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.player_stats(player);
            }
        }

        public void fire_room_enter(RoomChangeEventData data) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.room_enter(data);
            }
        }

        public void fire_room_leave(RoomChangeEventData data) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.room_leave(data);
            }
        }

        public void fire_table_join_fail(String error) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_join_fail(error);
            }
        }

        public void fire_table_joined(int table_index) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_joined(table_index);
            }
        }

        public void fire_table_launch_fail(ErrorEventData data) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_launch_fail(data);
            }
        }

        public void fire_table_launched() {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_launched();
            }
        }

        public void fire_table_leave_fail(String error) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_leave_fail(error);
            }
        }

        public void fire_table_left(TableLeaveEventData data) {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_left(data);
            }
        }

        public void fire_table_list() {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_list();
            }
        }

        public void fire_table_update() {
            // Java 5 inferred type
            RoomListener[] listenerArry = listeners
                    .getListeners(RoomListener.class);
            for (int i = 0; i < listenerArry.length; i++) {
                RoomListener listener = listenerArry[i];
                listener.table_update();
            }
        }
    }
}
