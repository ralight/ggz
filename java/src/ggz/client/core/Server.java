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

import ggz.common.ClientReqError;

import java.io.IOException;
import java.net.Socket;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Locale;
import java.util.ResourceBundle;
import java.util.logging.Logger;

import javax.swing.event.EventListenerList;

public class Server {
    private static final Logger log = Logger.getLogger(Server.class.getName());

    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.client.core.messages");

    /* Network object for doing net IO */
    private Net net;

    /* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
    private LoginType login_type;

    /* User handle on this server */
    private String handle;

    /* Password for this server (optional) */
    private String password;

    /* Email address for registration (optional) */
    private String email;

    /* Current state */
    private StateID state;

    /* List of game types */
    private GameType[] gametypes;

    /* List of rooms in this server */
    private ArrayList rooms;

    /* Current room on game server */
    private Room current_room;

    /* New (targeted) room - the room we're trying to move into. */
    private Room new_room;

    /* Current game on game server */
    private Game current_game;

    /* Game communications channel */
    Net channel;

    /*
     * Whether this is a primary ggzcore server connection or a channel.
     */
    private boolean is_channel;

    /*
     * Callbacks for server events. Every time an event happens all the hooks in
     * the list for that event are called.
     */
    private HookList event_hooks = new HookList();

    /*
     * This struct is used when queueing events. Events that may happen many
     * times during a single network read may instead be queued and later popped
     * off the queue when we leave the network code.
     */
    boolean queued_events_players_changed;

    static boolean reconnect_policy = false;

    static Server reconnect_server = null;

    static final int reconnect_timeout = 15;

    public Server(String host, int port, boolean use_tls) {
        /* Check for valid arguments */
        if (host == null) {
            throw new IllegalArgumentException("host cannot be null");
        }
        /* Set initial state */
        this.state = StateID.GGZ_STATE_OFFLINE;
        this.net = new Net(this, host, port, use_tls);
        clear();
    }

    public void set_logininfo(LoginType type, String handle, String password,
            String email) {
        /* Check for valid arguments */
        if (handle == null || (type == LoginType.GGZ_LOGIN && password == null))
            throw new IllegalArgumentException();

        /* Check for valid state */
        if (this.state == StateID.GGZ_STATE_OFFLINE
                || this.state == StateID.GGZ_STATE_CONNECTING
                || this.state == StateID.GGZ_STATE_ONLINE) {
            set_logintype(type);
            set_handle(handle);

            /* Password and email address may be null but we set it anyway. */
            set_password(password);
            set_email(email);
        } else {
            throw new IllegalStateException(this.state.toString());
        }
    }

    public String get_host() {
        if (this.net != null) {
            return this.net.get_host();
        }
        return null;
    }

    public int get_port() {
        if (this.net != null) {
            return this.net.get_port();
        }
        return -1;
    }

    /*
     * get_fd() - Get a copy of the network socket Receives:
     * 
     * Returns: int : network socket fd
     * 
     * Note: this is for detecting network data arrival only. Do *NOT* attempt
     * to write to this fd.
     */
    Socket get_fd() {
        if (this.net != null) {
            return this.net.get_fd();
        }
        return null;
    }

    public Room get_nth_room(int num) {
        /* Num is so it is always >= 0. */
        if (num < get_num_rooms()) {
            return (Room) this.rooms.get(num);
        }
        return null;
    }

    public GameType get_nth_gametype(int num) {
        /* Num is so it is always >= 0. */
        if (num < get_num_gametypes()) {
            return this.gametypes[num];
        }
        return null;
    }

    public boolean is_online() {
        if (this.state == StateID.GGZ_STATE_OFFLINE
                || this.state == StateID.GGZ_STATE_CONNECTING
                || this.state == StateID.GGZ_STATE_RECONNECTING) {
            return false;
        }
        return true;
    }

    public boolean is_logged_in() {
        if (this.state == StateID.GGZ_STATE_OFFLINE
                || this.state == StateID.GGZ_STATE_CONNECTING
                || this.state == StateID.GGZ_STATE_RECONNECTING
                || this.state == StateID.GGZ_STATE_ONLINE
                || this.state == StateID.GGZ_STATE_LOGGING_IN) {
            return false;
        }
        return true;
    }

    public boolean is_in_room() {
        if (this.state == StateID.GGZ_STATE_OFFLINE
                || this.state == StateID.GGZ_STATE_CONNECTING
                || this.state == StateID.GGZ_STATE_RECONNECTING
                || this.state == StateID.GGZ_STATE_ONLINE
                || this.state == StateID.GGZ_STATE_LOGGING_IN
                || this.state == StateID.GGZ_STATE_LOGGED_IN
                || this.state == StateID.GGZ_STATE_ENTERING_ROOM
                || this.state == StateID.GGZ_STATE_LOGGING_OUT) {
            return false;
        }
        return true;
    }

    public boolean is_at_table() {
        if (this.state == StateID.GGZ_STATE_AT_TABLE
                || this.state == StateID.GGZ_STATE_LEAVING_TABLE) {
            return true;
        }
        return false;
    }

    public void connect() {
        if (this.net == null)
            throw new IllegalStateException("net is null");

        check_state(TransID.GGZ_TRANS_CONN_TRY);

        change_state(TransID.GGZ_TRANS_CONN_TRY);
        try {
            this.net.connect();
            event(ServerEvent.GGZ_CONNECTED, null);
        } catch (Throwable e) {
            change_state(TransID.GGZ_TRANS_CONN_FAIL);
            event(ServerEvent.GGZ_CONNECT_FAIL, e.toString());
        }

    }

    public void create_channel() {
        if (this.net != null && this.state == StateID.GGZ_STATE_IN_ROOM) {
            String host;
            int port;
            String errmsg;

            host = this.net.get_host();
            port = this.net.get_port();
            this.channel = new Net(this, host, port, false);

            try {
                this.channel.connect();
                log.fine("Channel created");
                event(ServerEvent.GGZ_CHANNEL_CONNECTED, null);
            } catch (IOException e) {
                log.fine("Channel creation failed");
                errmsg = e.toString();
                event(ServerEvent.GGZ_CHANNEL_FAIL, errmsg);
            }

        } else
            throw new IllegalStateException(this.state.toString());
    }

    /**
     * Used by game clients that need to connect themselves.
     * 
     * @param host
     * @param port
     * @param handle
     * @return the socket that connects the game server with the game client.
     */
    public static Socket channel_connect(String host, int port, String handle)
            throws IOException {
        /*
         * Hack, hack, hack. The below really needs to be fixed up with some
         * error handling. There's also a major problem that it blocks the whole
         * time while the connection is being made.
         */
        Server server = new Server(host, port, false);
        server.is_channel = true;
        server.channel = server.net;
        // server->channel_complete = server->channel_failed = 0;
        server.set_logininfo(LoginType.GGZ_LOGIN_GUEST, handle, null, null);

        ServerListener monitor = new ServerAdapter() {
            public void server_logged_out() {
                synchronized (this) {
                    notify();
                }
            }
        };
        server.add_event_hook(monitor);
        synchronized (monitor) {
            server.connect();
            try {
                monitor.wait();
            } catch (InterruptedException ex) {
                throw new IOException("Failed to create channel.");
            }
        }
        return server.net.get_fd();
    }

    public void login() throws IOException {
        /* Return nothing if we didn't get the necessary info */
        if (this.handle == null)
            throw new IllegalStateException("handle has not been set yet");

        // if (this.state != StateID.GGZ_STATE_ONLINE)
        // throw new IllegalStateException(
        // "Attempt to logon before server is online, call connect()"
        // + " first and then wait for server_negotiate()");
        check_state(TransID.GGZ_TRANS_LOGIN_TRY);

        if (this.login_type == LoginType.GGZ_LOGIN && this.password == null)
            throw new IllegalStateException("password has not bee set yet");

        log.fine("Login (" + this.login_type + "), " + this.handle + ", "
                + this.password);

        net.send_login(this.login_type, this.handle, this.password, this.email,
                Locale.getDefault().getLanguage());

        change_state(TransID.GGZ_TRANS_LOGIN_TRY);
    }

    public void motd() throws IOException {
        if (is_logged_in())
            load_motd();
        else
            throw new IllegalStateException(this.state.toString());
    }

    public void list_rooms(boolean verbose) throws IOException {
        if (is_logged_in())
            load_roomlist(verbose);
        else
            throw new IllegalStateException(this.state.toString());
    }

    public void list_gametypes(boolean verbose) throws IOException {
        if (is_logged_in())
            load_typelist(verbose);
        else
            throw new IllegalStateException(this.state.toString());
    }

    public void join_room(int room_num) throws IOException {
        check_state(TransID.GGZ_TRANS_ENTER_TRY);
        if (room_num < get_num_rooms()) {
            int room_id;
            Room room_to_join;

            /* We need to send the room's ID on the server */
            room_to_join = get_nth_room(room_num);
            room_id = room_to_join.get_id();

            log.fine("Moving to room " + room_num);

            change_state(TransID.GGZ_TRANS_ENTER_TRY);
            net.send_join_room(room_id);
            this.new_room = room_to_join;
        }
    }

    public void logout() throws IOException {
        check_state(TransID.GGZ_TRANS_LOGOUT_TRY);
        log.fine("Logging out");
        change_state(TransID.GGZ_TRANS_LOGOUT_TRY);
        net.send_logout();
    }

    void disconnect() {
        if (this.state != StateID.GGZ_STATE_OFFLINE
                && this.state != StateID.GGZ_STATE_RECONNECTING) {
            log.fine("Disconnecting");
            net.disconnect();

            /* Force the server object into the offline state */
            this.state = StateID.GGZ_STATE_OFFLINE;
            event(ServerEvent.GGZ_STATE_CHANGE, null);

        } else
            throw new IllegalStateException(this.state.toString());
    }

    Net get_net() {
        return this.net;
    }

    public LoginType get_type() {
        return this.login_type;
    }

    public String get_handle() {
        return this.handle;
    }

    String get_password() {
        return this.password;
    }

    public Socket get_channel() {
        if (this.channel != null) {
            return this.channel.get_fd();
        }
        return null;
    }

    public StateID get_state() {
        return this.state;
    }

    public boolean get_tls() {
        boolean tls;

        tls = false;
        if (this.net != null) {
            tls = this.net.get_tls();
        }
        return tls;
    }

    public int get_num_players() {
        int total = 0;

        for (int i = 0; i < get_num_rooms(); i++) {
            Room nth_room = get_nth_room(i);

            /*
             * Each room should track an approximate number of players inside it
             * (or 0 which is just as good). We just total those up.
             */
            total += nth_room.get_num_players();
        }

        return total;
    }

    public int get_num_rooms() {
        return this.rooms == null ? 0 : this.rooms.size();
    }

    public Room get_cur_room() {
        return this.current_room;
    }

    public Room get_room_by_id(int id) {
        int i;

        for (i = 0; i < this.get_num_rooms(); i++) {
            Room room = get_nth_room(i);
            if (room.get_id() == id)
                return room;
        }

        return null;
    }

    public int get_num_gametypes() {
        return this.gametypes == null ? 0 : this.gametypes.length;
    }

    public GameType get_type_by_id(int id) {
        int i;

        for (i = 0; i < this.get_num_gametypes(); i++)
            if (this.gametypes[i].get_id() == id)
                return this.gametypes[i];

        return null;
    }

    public Game get_cur_game() {
        return this.current_game;
    }

    void set_cur_game(Game game) {
        this.current_game = game;
    }

    private void set_logintype(LoginType type) {
        this.login_type = type;
    }

    private void set_handle(String handle) {
        this.handle = handle;
    }

    void set_password(String password) {
        this.password = password;
    }

    private void set_email(String email) {
        this.email = email;
    }

    void set_cur_room(Room room) {
        Room old = get_cur_room();
        int num_players = 0;

        /* Stop monitoring old room/start monitoring new one */
        if (old != null) {
            num_players = old.get_num_players();
            old.set_monitor(false);
        }

        this.current_room = room;

        room.set_monitor(true);
        /* No need to update the player_count in the new room. */

        if (old != null) {
            /*
             * This is done last so this room will no longer be the current one.
             */
            old.set_players(num_players - 1);
        }
    }

    void set_negotiate_status(Net net, ClientReqError status)
            throws IOException {
        if (net != this.net && net != this.channel) {
            net_error("Unknown negotation");
        } else if (!this.is_channel) {
            if (net == this.channel) {
                channel_negotiate_status(status);
            } else {
                main_negotiate_status(status);
            }
        } else {
            channel_negotiate_status(status);
        }
    }

    void set_login_status(ClientReqError status) {
        log.fine("Status of login: " + status);

        check_state(new TransID[] { TransID.GGZ_TRANS_LOGIN_OK,
                TransID.GGZ_TRANS_LOGIN_FAIL });

        if (status == ClientReqError.E_OK) {
            change_state(TransID.GGZ_TRANS_LOGIN_OK);
            event(ServerEvent.GGZ_LOGGED_IN, null);
        } else {
            ErrorEventData error = new ErrorEventData(status);

            if (status == ClientReqError.E_ALREADY_LOGGED_IN) {
                error.message = messages
                        .getString("Server.LoginError.AlreadyLoggedIn");
            } else if (status == ClientReqError.E_USR_LOOKUP) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.UsrLookup"),
                        new String[] { handle });
            } else if (status == ClientReqError.E_USR_FOUND) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.UsrFound"),
                        new String[] { handle });
            } else if (status == ClientReqError.E_USR_TAKEN) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.UsrTaken"),
                        new String[] { handle });
            } else if (status == ClientReqError.E_USR_TYPE) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.UsrType"),
                        new String[] { handle });
            } else if (status == ClientReqError.E_TOO_LONG) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.TooLong"),
                        new String[] { handle });
            } else if (status == ClientReqError.E_BAD_OPTIONS) {
                error.message = messages
                        .getString("Server.LoginError.BadOptions");
            } else if (status == ClientReqError.E_BAD_USERNAME) {
                error.message = MessageFormat.format(messages
                        .getString("Server.LoginError.BadUsername"),
                        new String[] { handle });
            } else {
                error.message = messages.getString("Server.LoginError.Unknown");
            }
            change_state(TransID.GGZ_TRANS_LOGIN_FAIL);
            event(ServerEvent.GGZ_LOGIN_FAIL, error);
        }
    }

    void set_room_join_status(ClientReqError status) {
        check_state(new TransID[] { TransID.GGZ_TRANS_ENTER_OK,
                TransID.GGZ_TRANS_ENTER_FAIL });
        if (status == ClientReqError.E_OK) {
            set_cur_room(this.new_room);
            change_state(TransID.GGZ_TRANS_ENTER_OK);
            event(ServerEvent.GGZ_ENTERED, null);
        } else {
            ErrorEventData error = new ErrorEventData(status);

            if (status == ClientReqError.E_ROOM_FULL) {
                error.message = MessageFormat.format(messages
                        .getString("Server.RoomJoinError.RoomFull"),
                        new Object[] { this.new_room.get_name() });
            } else if (status == ClientReqError.E_AT_TABLE) {
                error.message = messages
                        .getString("Server.RoomJoinError.AtTable");
            } else if (status == ClientReqError.E_IN_TRANSIT) {
                error.message = messages
                        .getString("Server.RoomJoinError.InTransit");
            } else if (status == ClientReqError.E_BAD_OPTIONS) {
                error.message = messages
                        .getString("Server.RoomJoinError.BadOptions");
            } else {
                error.message = messages
                        .getString("Server.RoomJoinError.Unknown");
            }

            change_state(TransID.GGZ_TRANS_ENTER_FAIL);
            event(ServerEvent.GGZ_ENTER_FAIL, error);
        }
    }

    void set_table_launching() {
        check_state(TransID.GGZ_TRANS_LAUNCH_TRY);
        change_state(TransID.GGZ_TRANS_LAUNCH_TRY);
    }

    void set_table_joining() {
        check_state(TransID.GGZ_TRANS_JOIN_TRY);
        change_state(TransID.GGZ_TRANS_JOIN_TRY);
    }

    void check_table_leaving_state() {
        check_state(TransID.GGZ_TRANS_LEAVE_TRY);
    }

    void set_table_leaving() {
        change_state(TransID.GGZ_TRANS_LEAVE_TRY);
    }

    void set_table_launch_status(ClientReqError status) {
        if (status == ClientReqError.E_OK) {
            check_state(TransID.GGZ_TRANS_LAUNCH_OK);
            change_state(TransID.GGZ_TRANS_LAUNCH_OK);
        } else {
            check_state(TransID.GGZ_TRANS_LAUNCH_FAIL);
            change_state(TransID.GGZ_TRANS_LAUNCH_FAIL);
            if (this.current_game != null)
                this.current_game.abort_game();
        }
    }

    void set_table_join_status(ClientReqError status) {
        if (status == ClientReqError.E_OK) {
            check_state(TransID.GGZ_TRANS_JOIN_OK);
            change_state(TransID.GGZ_TRANS_JOIN_OK);
        } else {
            check_state(TransID.GGZ_TRANS_JOIN_FAIL);
            change_state(TransID.GGZ_TRANS_JOIN_FAIL);
            if (this.current_game != null)
                this.current_game.abort_game();
        }
    }

    void set_table_leave_status(ClientReqError status) {
        if (status == ClientReqError.E_OK) {
            check_state(TransID.GGZ_TRANS_LEAVE_OK);
            change_state(TransID.GGZ_TRANS_LEAVE_OK);
            if (this.current_game != null)
                this.current_game.abort_game();
        } else {
            check_state(TransID.GGZ_TRANS_LEAVE_FAIL);
            change_state(TransID.GGZ_TRANS_LEAVE_FAIL);
        }
    }

    void session_over(Net source) {
        if (source != this.net && source != this.channel)
            return;

        if (!this.is_channel) {
            if (source == this.channel) {
                // Channel (requested by ggzcore) is ready!
                if (this.current_game != null)
                    this.current_game.set_server_fd(get_channel());
                event(ServerEvent.GGZ_CHANNEL_READY, null);
            } else {
                // Server is ending session.
                check_state(TransID.GGZ_TRANS_LOGOUT_OK);
                source.disconnect();
                clear();
                change_state(TransID.GGZ_TRANS_LOGOUT_OK);
                event(ServerEvent.GGZ_LOGOUT, null);
            }
        } else {
            // Channel (requested by ggzmod) is ready!
            // We have to go through all these steps to keep the state machine
            // happy.
            check_state(TransID.GGZ_TRANS_CONN_OK);
            change_state(TransID.GGZ_TRANS_CONN_OK);
            check_state(TransID.GGZ_TRANS_LOGOUT_TRY);
            change_state(TransID.GGZ_TRANS_LOGOUT_TRY);
            check_state(TransID.GGZ_TRANS_LOGOUT_OK);
            change_state(TransID.GGZ_TRANS_LOGOUT_OK);
            event(ServerEvent.GGZ_LOGOUT, null);
        }
    }

    public void log_session(String sendFile, String receiveFile)
            throws IOException {
        net.setSessionDumpFiles(sendFile, receiveFile);
    }

    /**
     * This is different to the C code since Java has better support for event
     * listeners.
     * 
     * @param l
     */
    public void add_event_hook(ServerListener l) {
        event_hooks.addServerListener(l);
    }

    /**
     * This is different to the C code since Java has better support for event
     * listeners.
     * 
     * @param l
     */
    public void remove_event_hook(ServerListener l) {
        event_hooks.removeServerListener(l);
    }

    private void load_motd() throws IOException {
        net.send_motd();
    }

    private void load_typelist(boolean verbose) throws IOException {
        net.send_list_types(verbose);
    }

    private void load_roomlist(boolean verbose) throws IOException {
        net.send_list_rooms(verbose);
    }

    void clear_reconnect() {
        /* Clear all server-internal members (reconnection only) */
        this.rooms = null;
        this.current_room = null;
        this.current_game = null;
        this.gametypes = null;
    }

    void clear() {
        /* Clear all members */
        this.is_channel = false;
        this.channel = null;
        this.handle = null;
        this.password = null;
        this.rooms = null;
        this.current_room = null;
        this.current_game = null;
        this.gametypes = null;

        /*
         * Don't remove listeners. ServerListener[] listenerArray =
         * (ServerListener[]) event_hooks.listeners
         * .getListeners(ServerListener.class); for (int i = 0; i <
         * listenerArray.length; i++) { ServerListener listener =
         * listenerArray[i]; event_hooks.removeServerListener(listener); }
         */
    }

    /* Static functions internal to this file */

    void main_negotiate_status(ClientReqError status) {
        check_state(new TransID[] { TransID.GGZ_TRANS_CONN_OK,
                TransID.GGZ_TRANS_CONN_FAIL });
        if (status == ClientReqError.E_OK) {
            change_state(TransID.GGZ_TRANS_CONN_OK);
            event(ServerEvent.GGZ_NEGOTIATED, null);
        } else {
            change_state(TransID.GGZ_TRANS_CONN_FAIL);
            event(ServerEvent.GGZ_NEGOTIATE_FAIL, "Protocol mismatch");
        }
    }

    void channel_negotiate_status(ClientReqError status) throws IOException {
        if (status == ClientReqError.E_OK) {
            this.channel.send_channel(this.handle);
            this.channel.send_logout();
        } else {
            if (!this.is_channel) {
                event(ServerEvent.GGZ_CHANNEL_FAIL, "Protocol mismatch");
            }
        }
    }

    void init_roomlist(int num) {
        this.rooms = new ArrayList(num);
    }

    void add_room(Room room) {
        this.rooms.add(room);
    }

    void init_typelist(int num) {
        this.gametypes = new GameType[num];
    }

    void delete_room(Room room) {
        this.rooms.remove(room);
    }

    void add_type(GameType type) {
        int i = 0;

        /* Find first open spot and stick it in */
        while (i < this.get_num_gametypes()) {
            if (this.gametypes[i] == null) {
                this.gametypes[i] = type;
                break;
            }
            ++i;
        }
    }

    // TODO Support reconnect retries.
    // static void reconnect_alarm(int signal) {
    // try {
    // reconnect_server.net.connect();
    // reconnect_server.state = StateID.GGZ_STATE_ONLINE;
    // reconnect_server.event(ServerEvent.GGZ_CONNECTED, null);
    // } catch (IOException e) {
    // reconnect_server.state = StateID.GGZ_STATE_RECONNECTING;
    // alarm(reconnect_timeout);
    // }
    // }

    /**
     * Checks that the server is in a valid state to perform any of the given
     * transitions. This method is intended as a guard in methods that change
     * state to prevent them performing an action that then leaves the server in
     * an invalid state.
     */
    private void check_state(TransID trans) {
        if (this.state == trans.state_transition(this.state)) {
            throw new IllegalStateException(this.state.toString());
        }
    }

    private void check_state(TransID[] trans) {
        for (int i = 0; i < trans.length; i++) {
            check_state(trans[i]);
        }
    }

    void change_state(TransID trans) {
        String host;
        int port;
        boolean use_tls;

        if (trans == TransID.GGZ_TRANS_NET_ERROR
                || trans == TransID.GGZ_TRANS_PROTO_ERROR) {
            if (reconnect_policy) {
                reconnect_server = this;
                host = this.net.get_host();
                port = this.net.get_port();
                use_tls = this.net.get_tls();
                this.net = new Net(this, host, port, use_tls);

                clear_reconnect();

                this.state = StateID.GGZ_STATE_RECONNECTING;
                event(ServerEvent.GGZ_STATE_CHANGE, null);
                // Unix stuff that we need to simulate in Java one day when I
                // can be bothered.
                // signal(SIGALRM, reconnect_alarm);
                // alarm(reconnect_timeout);
                return;
            }
        }

        StateID nextState = trans.state_transition(this.state);
        if (nextState == this.state) {
            log.warning("No transitions found TransID=" + trans
                    + " Current State=" + this.state);
        } else {
            log.fine("State transition " + this.state + " -> " + nextState);
            this.state = nextState;
            event(ServerEvent.GGZ_STATE_CHANGE, null);
        }
    }

    void event(ServerEvent id, Object data) {
        if (id == ServerEvent.GGZ_CONNECTED) {
            event_hooks.fire_connected();
        } else if (id == ServerEvent.GGZ_CONNECT_FAIL) {
            event_hooks.fire_connect_fail((String) data);
        } else if (id == ServerEvent.GGZ_NEGOTIATED) {
            event_hooks.fire_negotiated();
        } else if (id == ServerEvent.GGZ_NEGOTIATE_FAIL) {
            event_hooks.fire_negotiate_fail(data.toString());
        } else if (id == ServerEvent.GGZ_LOGGED_IN) {
            event_hooks.fire_logged_in();
        } else if (id == ServerEvent.GGZ_LOGIN_FAIL) {
            event_hooks.fire_login_fail((ErrorEventData) data);
        } else if (id == ServerEvent.GGZ_MOTD_LOADED) {
            event_hooks.fire_motd_loaded((MotdEventData) data);
        } else if (id == ServerEvent.GGZ_ROOM_LIST) {
            event_hooks.fire_room_list();
        } else if (id == ServerEvent.GGZ_TYPE_LIST) {
            event_hooks.fire_type_list();
        } else if (id == ServerEvent.GGZ_SERVER_PLAYERS_CHANGED) {
            event_hooks.fire_server_players_changed();
        } else if (id == ServerEvent.GGZ_ENTERED) {
            event_hooks.fire_entered();
        } else if (id == ServerEvent.GGZ_ENTER_FAIL) {
            event_hooks.fire_enter_fail((ErrorEventData) data);
        } else if (id == ServerEvent.GGZ_LOGOUT) {
            event_hooks.fire_logout();
        } else if (id == ServerEvent.GGZ_NET_ERROR) {
            event_hooks.fire_net_error((String) data);
        } else if (id == ServerEvent.GGZ_PROTOCOL_ERROR) {
            event_hooks.fire_protocol_error((String) data);
        } else if (id == ServerEvent.GGZ_CHAT_FAIL) {
            event_hooks.fire_chat_fail((ErrorEventData) data);
        } else if (id == ServerEvent.GGZ_STATE_CHANGE) {
            event_hooks.fire_state_change();
        } else if (id == ServerEvent.GGZ_CHANNEL_CONNECTED) {
            event_hooks.fire_channel_connected();
        } else if (id == ServerEvent.GGZ_CHANNEL_READY) {
            event_hooks.fire_channel_ready();
        } else if (id == ServerEvent.GGZ_CHANNEL_FAIL) {
            event_hooks.fire_channel_fail((String) data);
        } else if (id == ServerEvent.GGZ_SERVER_ROOMS_CHANGED) {
            event_hooks.fire_server_rooms_changed();
        } else {
            throw new IllegalArgumentException("Unhandled ServerEvent: " + id);
        }
    }

    void queue_players_changed() {
        /*
         * This queues a GGZ_SERVER_PLAYERS_CHANGED event. The event is
         * activated when the network code is exited so that it's not needlessly
         * activated multiple times, which would also cause the number to be
         * inaccurate (but only for a short time).
         */
        this.queued_events_players_changed = true;
    }

    void process_player_changed_queue() {
        if (this.queued_events_players_changed) {
            // These next two lines are from the C read_data() function, which
            // is not called in the Java port so we do it this way.
            event(ServerEvent.GGZ_SERVER_PLAYERS_CHANGED, null);
            this.queued_events_players_changed = false;
        }
    }

    void net_error(String message) {
        change_state(TransID.GGZ_TRANS_NET_ERROR);
        event(ServerEvent.GGZ_NET_ERROR, message);
    }

    void protocol_error(String message) {
        log.fine("Protocol error: disconnecting");
        this.net.disconnect();
        change_state(TransID.GGZ_TRANS_PROTO_ERROR);
        event(ServerEvent.GGZ_PROTOCOL_ERROR, message);
        // We're now also logged out.
        clear();
        event(ServerEvent.GGZ_LOGOUT, null);
    }

    static void set_reconnect() {
        throw new UnsupportedOperationException(
                "Auto reconnect is not supported in the Java port yet.");
        // reconnect_policy = true;
    }

    protected class HookList {
        protected EventListenerList listeners = new EventListenerList();

        public void addServerListener(ServerListener l) {
            listeners.add(ServerListener.class, l);
        }

        public void removeServerListener(ServerListener l) {
            listeners.remove(ServerListener.class, l);
        }

        public void fire_connected() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_connected();
            }
        }

        public void fire_connect_fail(String error) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_connect_fail(error);
            }
        }

        public void fire_channel_connected() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_channel_connected();
            }
        }

        public void fire_channel_fail(String error) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_channel_fail(error);
            }
        }

        public void fire_channel_ready() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_channel_ready();
            }
        }

        public void fire_chat_fail(ErrorEventData data) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_chat_fail(data);
            }
        }

        public void fire_enter_fail(ErrorEventData data) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_enter_fail(data);
            }
        }

        public void fire_entered() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_enter_ok();
            }
        }

        public void fire_logged_in() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_login_ok();
            }
        }

        public void fire_login_fail(ErrorEventData data) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_login_fail(data);
            }
        }

        public void fire_logout() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_logged_out();
            }
        }

        public void fire_motd_loaded(MotdEventData data) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_motd_loaded(data);
            }
        }

        public void fire_negotiate_fail(String error) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_negotiate_fail(error);
            }
        }

        public void fire_negotiated() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_negotiated();
            }
        }

        public void fire_net_error(String error) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_net_error(error);
            }
        }

        public void fire_protocol_error(String error) {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_protocol_error(error);
            }
        }

        public void fire_room_list() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_list_rooms();
            }
        }

        public void fire_server_players_changed() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_players_changed();
            }
        }

        public void fire_state_change() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_state_changed();
            }
        }

        public void fire_type_list() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_list_types();
            }
        }

        public void fire_server_rooms_changed() {
            // Java 5 inferred type
            ServerListener[] listenerArray = (ServerListener[]) listeners
                    .getListeners(ServerListener.class);
            for (int i = 0; i < listenerArray.length; i++) {
                ServerListener listener = listenerArray[i];
                listener.server_rooms_changed();
            }
        }

    }

}
