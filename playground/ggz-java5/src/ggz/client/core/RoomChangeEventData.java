package ggz.client.core;

/** @brief The data associated with GGZ_ROOM_ENTER/GGZ_ROOM_LEAVE events. */
public class RoomChangeEventData {

    /** @brief The name of the player entering/leaving. */
    public String player_name;

    /**
     * @brief The room we are entering.
     * 
     * This may be -1 if the player is leaving the server, or -2 if the info is
     * unknown.
     */
    public int to_room;

    /**
     * @brief The room the player is leaving.
     * 
     * This may be -1 if the player is just entering the server, or -2 if the
     * info is unknown.
     */
    public int from_room;
}
