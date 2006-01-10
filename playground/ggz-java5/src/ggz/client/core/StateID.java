package ggz.client.core;

/**
 * @brief The states a server connection may be in.
 * 
 * On the client side, a simplistic state maching is used to tell what's going
 * on. A game client should usually consult the current state when determining
 * what actions are possible.
 */
public enum StateID {
    /** Not connected (at all) */
    GGZ_STATE_OFFLINE,
    /** In the process of connecting. */
    GGZ_STATE_CONNECTING,
    /** Continuous reconnection attempts. */
    GGZ_STATE_RECONNECTING,
    /** Connected, but not doing anything. */
    GGZ_STATE_ONLINE,
    /** In the process of logging in. */
    GGZ_STATE_LOGGING_IN,
    /** Online and logged in! */
    GGZ_STATE_LOGGED_IN,
    /** Moving into a room. */
    GGZ_STATE_ENTERING_ROOM,
    /** Online, logged in, and in a room. */
    GGZ_STATE_IN_ROOM,
    /** Moving between rooms. */
    GGZ_STATE_BETWEEN_ROOMS,
    /** Trying to launch a table. */
    GGZ_STATE_LAUNCHING_TABLE,
    /** Trying to join a table. */
    GGZ_STATE_JOINING_TABLE,
    /** Online, loggied in, in a room, at a table. */
    GGZ_STATE_AT_TABLE,
    /** Waiting to leave a table. */
    GGZ_STATE_LEAVING_TABLE,
    /** In the process of logging out. */
    GGZ_STATE_LOGGING_OUT

}
