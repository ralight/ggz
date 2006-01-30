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

import ggz.client.mod.Mod;
import ggz.client.mod.ModGGZ;
import ggz.client.mod.ModState;
import ggz.client.mod.ModTransaction;
import ggz.client.mod.ModTransactionHandler;
import ggz.client.mod.Seat;
import ggz.client.mod.SpectatorSeat;
import ggz.common.ChatType;
import ggz.common.SeatType;

import java.io.IOException;
import java.net.Socket;
import java.util.List;
import java.util.logging.Logger;

/*
 * The Game object manages information about a particular running game
 */
public class Game implements ModTransactionHandler {
    private static final Logger log = Logger.getLogger(Game.class.getName());

    /* Pointer to module this game is playing */
    private Module module;

    /* Room events */
    // HookList *event_hooks[sizeof(events) /
    // sizeof(events[0])];
    private GameEventListener event_hooks;

    /* Game module connection */
    private ModGGZ client;

    /* The server data for this game. */
    private Server server;

    /* Are we playing or watching at our table? */
    private boolean spectating;

    /* What's our seat number? */
    private int my_seat_num;

    /* Which room this game is in. */
    private int room_id;

    /* The table ID for this game. */
    private int table_id;

    /* Publicly exported functions */

    public Game(Server server, Module module) {
        if (server == null) {
            throw new IllegalArgumentException("server cannot be null");
        }
        if (server.get_cur_room() == null) {
            throw new IllegalStateException(
                    "server does not have a current room");
        }
        if (server.get_cur_game() != null) {
            throw new IllegalStateException("server already has a current game");
        }

        if (module == null && !Module.is_embedded())
            throw new IllegalArgumentException();

        Room room = server.get_cur_room();

        this.server = server;
        this.room_id = room.get_id();
        this.table_id = -1;

        server.set_cur_game(this);

        this.module = module;

        log.fine("Initializing new game");

        /* Setup event hook list */
        // for (i = 0; i < _ggzcore_num_events; i++)
        // this.event_hooks[i] = _ggzcore_hook_list_init(i);
        /* Setup client module connection */
        this.client = new Mod();// ModType.GGZMOD_GGZ);
        // this.client.set_gamedata(this);
        // #ifdef HAVE_WINSOCK2_H
        // this.client.set_server_host(
        // server.get_host(),
        // server.get_port(),
        // server.get_handle());
        // #endif
        this.client.add_mod_listener(this);
        // this.client.set_player(server.get_handle(), false, -1);

        if (!Module.is_embedded())
            this.client.set_module(null, module.get_class_name());
    }

    /* Functions for attaching hooks to Game events */
    public void add_event_hook(GameEventListener listener)
    // const GameEvent event,
    // const HookFunc func)
    {
        if (this.event_hooks != null && listener != this.event_hooks) {
            throw new UnsupportedOperationException(
                    "Multiple listeners not supported yet.");
        }
        this.event_hooks = listener;
    }

    /* Functions for removing hooks from Game events */
    public void remove_event_hook(GameEventListener l) {
        if (this.event_hooks == l) {
            this.event_hooks = null;
        }
    }

    // Socket get_control_fd()
    // {
    // return this.client.get_fd();
    //
    // }

    /*
     * This function is called by ggzmod when the game state is changed.
     * 
     * Game state changes are all initiated by the game client through ggzmod.
     * So if we get here we just have to update ggzcore and the ggz client based
     * on what changes have already happened.
     */
    public void handle_state(ModState prev) throws IOException {
        ModState newState = this.client.get_state();

        log.fine("Game changing from state " + prev + " to " + newState);

        switch (prev) {
        case GGZMOD_STATE_CREATED:
            log.fine("game negotiated");
            event(GameEvent.GGZ_GAME_NEGOTIATED, null);
            if (newState != ModState.GGZMOD_STATE_CONNECTED) {
                log.severe("Game changed state from created to " + newState);
            }
            break;
        case GGZMOD_STATE_CONNECTED:
            log.fine("game playing");
            event(GameEvent.GGZ_GAME_PLAYING, null);
            if (newState != ModState.GGZMOD_STATE_WAITING
                    && newState != ModState.GGZMOD_STATE_PLAYING) {
                log.severe("Game changed state from connected to " + newState);
            }
            break;
        case GGZMOD_STATE_WAITING:
        case GGZMOD_STATE_PLAYING:
        case GGZMOD_STATE_DONE:
            break;
        }

        switch (newState) {
        case GGZMOD_STATE_CONNECTED:
        case GGZMOD_STATE_WAITING:
        case GGZMOD_STATE_PLAYING:
            break;
        case GGZMOD_STATE_DONE:
            abort_game();
            break;

        case GGZMOD_STATE_CREATED:
            /*
             * Leave the game running. This should never happen since this is
             * the initial state and we never return to it after leaving it.
             */
            log.severe("Game state changed to created!");
            break;

        }
    }

    public void handle_sit(int seat_num) throws IOException {
        Net net = this.server.get_net();
        ReseatType op;

        if (this.spectating)
            op = ReseatType.GGZ_RESEAT_SIT;
        else
            op = ReseatType.GGZ_RESEAT_MOVE;

        net.send_table_reseat(op, seat_num);
    }

    public void handle_stand() throws IOException {
        Net net = this.server.get_net();

        net.send_table_reseat(ReseatType.GGZ_RESEAT_STAND, -1);
    }

    public void handle_boot(String name) throws IOException {
        Net net = this.server.get_net();
        Room room = this.server.get_nth_room(this.room_id);
        Table table = room.get_table_by_id(this.table_id);
        int i;

        for (i = 0; i < table.get_num_seats(); i++) {
            TableSeat seat = table.get_nth_seat(i);

            if (seat.type != SeatType.GGZ_SEAT_PLAYER
                    || !seat.name.equals(name))
                continue;
            net.send_table_boot_update(table, seat);
            return;
        }

        for (i = 0; i < table.get_num_spectator_seats(); i++) {
            TableSeat spectator = table.get_nth_spectator_seat(i);

            if (!spectator.name.equals(name))
                continue;
            net.send_table_boot_update(table, spectator);
            return;
        }

        /*
         * If the player has already left, we won't find them and we'll end up
         * down here.
         */
    }

    public void handle_seatchange(ModTransaction t, int seat_num)
            throws IOException {
        Net net = this.server.get_net();
        TableSeat seat = new TableSeat(seat_num, null, null);
        Room room = this.server.get_nth_room(this.room_id);
        Table table = room.get_table_by_id(this.table_id);

        if (t == ModTransaction.GGZMOD_TRANSACTION_OPEN)
            seat.type = SeatType.GGZ_SEAT_OPEN;
        else
            seat.type = SeatType.GGZ_SEAT_BOT;

        net.send_table_seat_update(table, seat);
    }

    public void handle_chat(String chat) throws IOException {
        Room room = this.server.get_cur_room();

        room.chat(ChatType.GGZ_CHAT_TABLE, null, chat);
    }

    public void handle_info(int seat_num) throws IOException {
        Net net = this.server.get_net();

        net.send_player_info(seat_num);
    }

    void set_table(int room_id, int table_id) {
        Room room;
        Table table;
        int num_seats, i;

        /* FIXME */
        assert (this.room_id == room_id);
        assert (this.table_id < 0 || this.table_id == table_id);

        room = server.get_cur_room();
        assert (room.get_id() == room_id);

        this.table_id = table_id;
        table = room.get_table_by_id(table_id);
        assert (table != null && table.get_id() == table_id);

        num_seats = table.get_num_seats();
        for (i = 0; i < num_seats; i++) {
            TableSeat seat = table.get_nth_seat(i);

            set_seat(seat);
        }

        num_seats = table.get_num_spectator_seats();
        for (i = 0; i < num_seats; i++) {
            TableSeat seat = table.get_nth_spectator_seat(i);

            set_spectator_seat(seat);
        }
    }

    void set_seat(TableSeat seat) {
        Seat mseat = new Seat(seat.index, seat.type, seat.name);
        this.client.set_seat(mseat);
    }

    void set_spectator_seat(TableSeat seat) {
        SpectatorSeat mseat = new SpectatorSeat(seat.index, seat.name);
        this.client.set_spectator_seat(mseat);
    }

    void set_player(boolean is_spectator, int seat_num) {
        if (this.spectating == is_spectator && this.my_seat_num == seat_num)
            return;

        this.spectating = is_spectator;
        this.my_seat_num = seat_num;

        this.client.set_player(server.get_handle(), is_spectator, seat_num);
    }

    void set_info(int num, List infos) {
        this.client.set_info(num, infos);
    }

    void inform_chat(String player, String msg) {
        this.client.inform_chat(player, msg);
    }

    public boolean is_spectator() {
        return this.spectating;
    }

    public int get_seat_num() {
        return this.my_seat_num;
    }

    public int get_room_id() {
        return this.room_id;
    }

    public int get_table_id() {
        return this.table_id;
    }

    /*
     * Called when the game client fails. Most likely the user has just exited.
     * Make sure ggzd knows we've left the table.
     */
    private void abort_game() throws IOException {
        // TableLeaveEventData event_data = new TableLeaveEventData(
        // LeaveType.GGZ_LEAVE_NORMAL, null);
        Room room = server.get_cur_room();

        /*
         * This would be called automatically later (several times in fact), but
         * doing it now is safe enough and starts the ball rolling.
         */
        this.client.disconnect();

        // This is fired when Net parses the table leave response so don't fire
        // it prematurely here.
        // if (room != null) {
        // room.table_event(RoomEvent.GGZ_TABLE_LEFT, event_data);
        // }

        if (room != null && server.get_state() == StateID.GGZ_STATE_AT_TABLE) {
            room.leave_table(true);
        }

        server.set_cur_game(null);
    }

    // public void disconnect() throws IOException{
    // client.disconnect();
    // }

    public void set_server_fd(Socket fd) throws IOException {
        this.client.set_server_fd(fd);
    }

    Module get_module() {
        return this.module;
    }

    private void event(GameEvent event, Object data) {
        if (event_hooks == null)
            return;

        switch (event) {
        case GGZ_GAME_LAUNCH_FAIL:
            event_hooks.game_launch_fail((Exception) data);
            break;
        case GGZ_GAME_LAUNCHED:
            event_hooks.game_launched();
            break;
        case GGZ_GAME_NEGOTIATE_FAIL:
            event_hooks.game_negotiate_fail();
            break;
        case GGZ_GAME_NEGOTIATED:
            event_hooks.game_negotiated();
            break;
        case GGZ_GAME_PLAYING:
            event_hooks.game_playing();
            break;
        }
    }

    public void launch() {
        if ((this.module != null || Module.is_embedded())) {
            if (Module.is_embedded())
                log.fine("Launching embedded game");
            else
                log.fine("Launching game of " + this.module.get_name());

            try {
                this.client.connect();
                log.fine("Launched game module");
                event(GameEvent.GGZ_GAME_LAUNCHED, null);
            } catch (Exception e) {
                log.severe("Failed to connect to game module");
                try {
                    abort_game();
                } catch (IOException e2) {
                    // Ignore.
                }
                event(GameEvent.GGZ_GAME_LAUNCH_FAIL, e);
            }
        }
    }

}
