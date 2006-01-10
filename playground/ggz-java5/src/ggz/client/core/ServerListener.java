package ggz.client.core;

import java.util.EventListener;

/**
 * A GGZServerEvent is an event triggered by a communication from the server.
 * Each time an event occurs, the associated event handler will be called, and
 * will be passed the event data (a void*). Most events are generated as a
 * result of ggzcore_server_read_data.
 * 
 * @see ggzcore_server_add_event_hook
 */
public interface ServerListener extends EventListener {
    /**
     * We have just made a connection to the server. After this point the
     * server's socket should be accessible and should be monitored for data. It
     * happens in direct response to ggzcore_server_connect. Note that most
     * events after this will only happen by calling ggzcore_server_read_data on
     * the server's FD!
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_connect
     */
    public void server_connected();

    /**
     * Error: we have failed to connect to the server. This is generated in
     * place of GGZ_CONNECTED if the connection could not be made. The server
     * object is otherwise unaffected.
     * 
     * @param data
     *            An error string (created by strerror)
     * @see ggzcore_server_connect
     */
    public void server_connect_fail(String error);

    /**
     * We have negotiated a connection to the server. This will happen
     * automatically once a connection has been established, if the server
     * socket is monitored.
     * 
     * @note This just means we've determined ggzd is at the other end.
     * @param data
     *            NULL
     * @see ggzcore_server_read_data
     */
    public void server_negotiated();

    /**
     * Error: negotiation failure. Could be the wrong version. This will happen
     * in place of a GGZ_NEGOTIATED if the server could not be negotiated with.
     * 
     * @param data
     *            A useless error string.
     * @see ggzcore_server_read_data
     */
    public void server_negotiate_fail(String error);

    /**
     * We have successfully logged in. We can now start doing stuff. This will
     * not happen until the client sends their login information.
     * 
     * @see ggzcore_server_login
     * @param data
     *            NULL
     * @see ggzcore_server_read_data
     */
    public void server_login_ok();

    /**
     * Error: login failure. This will happen in place of GGZ_LOGGED_IN if the
     * login failed. The server object will be otherwise unaffected.
     * 
     * @param data
     *            A pointer to a GGZErrorEventData.
     * @see GGZErrorEventData
     * @see ggzcore_server_read_data
     */
    public void server_login_fail(ErrorEventData data);

    /**
     * The MOTD has been read from the server and can be displayed. The server
     * will send us the MOTD automatically after login; it can also be requested
     * by ggzcore_server_motd. It is up to the client whether or not to display
     * it. See the online documentation (somewhere?) about the MOTD markup
     * format.
     * 
     * @param data
     *            Pointer to a GGZMotdEventData including the full MOTD text.
     * @see ggzcore_server_motd
     * @todo The MOTD cannot be accessed outside of this event
     * @see ggzcore_server_read_data!
     */
    public void server_motd_loaded(MotdEventData data);

    /**
     * The room list arrived. This will only happen after the list is requested
     * by ggzcore_server_list_rooms(). The list may be accessed through
     * ggzcore_server_get_num_rooms() and ggzcore_server_get_nth_room(). Until
     * this event arrives these functions will be useless!
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_read_data
     */
    public void server_list_rooms();

    /**
     * The list of game types is available. This will only happen after the list
     * is requested by ggzcore_server_list_types(). The list may be accessed
     * through ggzcore_server_get_num_gametypes() and
     * ggzcore_server_get_nth_gametype(). Until this event arrives these
     * functions will be useless!
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_read_data
     */
    public void server_list_types();

    /**
     * The number of players on the server has changed. This event is issued
     * rather frequently every time players enter or leave.
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_get_num_players
     * @see ggzcore_server_read_data
     */
    public void server_players_changed();

    /**
     * We have successfully entered a room. This will be issued to tell us a
     * room join has succeeded, after it has been requested.
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_join_room
     * @see ggzcore_server_read_data
     */
    public void server_enter_ok();

    /**
     * Error: we have tried to enter a room and failed. This will be issued to
     * tell us a room join has failed.
     * 
     * @param data
     *            A pointer to a GGZErrorEventData.
     * @see GGZErrorEventData
     * @see ggzcore_server_join_room
     * @see ggzcore_server_read_data
     */
    public void server_enter_fail(ErrorEventData data);

    /**
     * Logged out of the server. This will happen when the server completes the
     * communication; usually after ggzcore_net_send_logout is called.
     * 
     * @param data
     *            NULL
     * @see ggzcore_server_read_data
     */
    public void server_logged_out();

    /**
     * Error: a network (transmission) error occurred. The server will
     * automatically disconnect.
     * 
     * @param data
     *            A generally unhelpful error string.
     * @see ggzcore_server_read_data
     */
    public void server_net_error(String error);

    /**
     * Error: a communication protocol error occured. This can happen in a
     * variety of situations when the server sends us something we can't handle.
     * The server will be automatically disconnected.
     * 
     * @param data
     *            A technical error string.
     * @see ggzcore_server_read_data
     */
    public void server_protocol_error(String error);

    /**
     * Error: A chat message could not be sent. This will happen when we try to
     * send a chat and the server rejects it.
     * 
     * @param data
     *            A pointer to a GGZErrorEventData.
     * @see GGZErrorEventData
     * @see ggzcore_server_read_data
     */
    public void server_chat_fail(ErrorEventData data);

    /**
     * The internal state of ggzcore has changed. This may happen at any time.
     * 
     * @param data
     *            NULL
     * @see GGZStateID
     * @see ggzcore_server_get_state
     */
    public void server_state_changed();

    /**
     * Status event: a requested direct game connection has been established. To
     * start a game (table), a channel must be created. This event will alert
     * that the channel has been established. The channel's FD should then be
     * monitored for input, which should then be passed back to the server
     * object for handling.
     * 
     * @param data
     *            NULL
     * @note This event is deprecated and should not be used.
     * @see ggzcore_server_get_channel
     * @see ggzcore_server_read_data
     */
    public void server_channel_connected();

    /**
     * Game channel is ready for read/write operations. After the channel has
     * been connected, if we continue to monitor the socket eventually it will
     * be negotiated and ready to use. At this point it is ready for the game
     * client to use.
     * 
     * @param data
     *            NULL
     * @note This event is deprecated and should not be used.
     * @see ggzcore_server_read_data
     */
    public void server_channel_ready();

    /**
     * Error: Failure during setup of direct connection to game server. If the
     * channel could not be prepared, this event will happen instead of
     * GGZ_CHANNEL_READY or GGZ_CHANNEL_CONNECTED event. At this point the
     * channel is no longer useful (I think).
     * 
     * @param data
     *            An unhelpful error string
     * @note This event is deprecated and should not be used.
     * @see ggzcore_server_read_data
     */
    public void server_channel_fail(String error);
}
