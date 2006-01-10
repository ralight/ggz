package ggz.common;

/**
 * @brief A player type.
 * 
 * The server will tell the client the type of each player.
 */
public enum PlayerType {
    /** A normal player is registered but has no special permission. */
    GGZ_PLAYER_NORMAL,

    /** A guest player is not registered. */
    GGZ_PLAYER_GUEST,

    /** An admin player is registered and has some special permissions. */
    GGZ_PLAYER_ADMIN,

    /** A bot is a special type of player. */
    GGZ_PLAYER_BOT,

    /** This is an unknown type of player. */
    GGZ_PLAYER_UNKNOWN;

    private static final String GUEST_PLAYER_NAME = "guest";

    private static final String NORMAL_PLAYER_NAME = "normal";

    private static final String ADMIN_PLAYER_NAME = "admin";

    private static final String BOT_PLAYER_NAME = "bot";

    private static final String UNKNOWN_PLAYER_NAME = "unknown";

    public String toString() {
        switch (this) {
        case GGZ_PLAYER_GUEST:
            return GUEST_PLAYER_NAME;
        case GGZ_PLAYER_NORMAL:
            return NORMAL_PLAYER_NAME;
        case GGZ_PLAYER_ADMIN:
            return ADMIN_PLAYER_NAME;
        case GGZ_PLAYER_BOT:
            return BOT_PLAYER_NAME;
        case GGZ_PLAYER_UNKNOWN:
            return UNKNOWN_PLAYER_NAME;
        }

        // ggz_error_msg("ggz_playertype_to_string: "
        // "invalid playertype %d given.", type);
        return UNKNOWN_PLAYER_NAME; /* ? */
    }

    public static PlayerType string_to_playertype(String type_str) {
        if (type_str == null) {
            return GGZ_PLAYER_UNKNOWN;
        }

        if (NORMAL_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_NORMAL;
        } else if (GUEST_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_GUEST;
        } else if (ADMIN_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_ADMIN;
        } else if (BOT_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_BOT;
        }

        return GGZ_PLAYER_UNKNOWN; /* ? */
    }
}
