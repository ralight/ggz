package ggz.common;

/**
 * @brief Seat status values.
 * 
 * Each "seat" at a table of a GGZ game can have one of these values. They are
 * used by the GGZ client, GGZ server, and game servers; their use in game
 * clients is completely optional.
 */
public enum SeatType {
    /** This seat does not exist. */
    GGZ_SEAT_NONE,
    /** The seat is open (unoccupied). */
    GGZ_SEAT_OPEN,
    /** The seat has a bot (AI) in it. */
    GGZ_SEAT_BOT,
    /** The seat has a regular player in it. */
    GGZ_SEAT_PLAYER,
    /** The seat is reserved for a player. */
    GGZ_SEAT_RESERVED,
    /** The seat is abandoned by a player. */
    GGZ_SEAT_ABANDONED;
    private static final String OPEN_SEAT_NAME = "open";

    private static final String BOT_SEAT_NAME = "bot";

    private static final String PLAYER_SEAT_NAME = "player";

    private static final String RESERVED_SEAT_NAME = "reserved";

    private static final String ABANDONED_SEAT_NAME = "abandoned";

    private static final String NONE_SEAT_NAME = "none";

    public static SeatType string_to_seattype(String type_str) {
        /*
         * If it doesn't match _exactly_ we return GGZ_SEAT_NONE. This is bad
         * for, say, user input, but perfectly acceptable as an inverse to
         * ggz_seattype_to_string().
         */
        if (type_str == null)
            return GGZ_SEAT_NONE;

        if (OPEN_SEAT_NAME.equals(type_str))
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

    public String toString() {
        switch (this) {
        case GGZ_SEAT_OPEN:
            return OPEN_SEAT_NAME;
        case GGZ_SEAT_BOT:
            return BOT_SEAT_NAME;
        case GGZ_SEAT_RESERVED:
            return RESERVED_SEAT_NAME;
        case GGZ_SEAT_PLAYER:
            return PLAYER_SEAT_NAME;
        case GGZ_SEAT_ABANDONED:
            return ABANDONED_SEAT_NAME;
        case GGZ_SEAT_NONE:
            return NONE_SEAT_NAME;
        }

        // ggz_error_msg("ggz_seattype_to_string: "
        // "invalid seattype %d given.", type);
        return NONE_SEAT_NAME;
    }
    
    public static SeatType valueOf(int ordinal) {
        return values()[ordinal];
    }
}
