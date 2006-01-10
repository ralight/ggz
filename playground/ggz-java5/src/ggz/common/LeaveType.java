package ggz.common;

/**
 * @brief A leave type.
 * 
 * When a player leaves a table, this is a possible reason for the leave.
 */
public enum LeaveType {
    /** A normal leave - at user request. */
    GGZ_LEAVE_NORMAL,

    /** The player has been booted from the table. */
    GGZ_LEAVE_BOOT,

    /** The game is over; the server exited normally. */
    GGZ_LEAVE_GAMEOVER,

    /** There was an error in the game server and it was terminated. */
    GGZ_LEAVE_GAMEERROR;

    private static final String NORMAL_LEAVETYPE_NAME = "normal";

    private static final String BOOT_LEAVETYPE_NAME = "boot";

    private static final String GAMEOVER_LEAVETYPE_NAME = "gameover";

    private static final String GAMEERROR_LEAVETYPE_NAME = "gameerror";

    public String toString() {
        switch (this) {
        case GGZ_LEAVE_NORMAL:
            return NORMAL_LEAVETYPE_NAME;
        case GGZ_LEAVE_BOOT:
            return BOOT_LEAVETYPE_NAME;
        case GGZ_LEAVE_GAMEOVER:
            return GAMEOVER_LEAVETYPE_NAME;
        case GGZ_LEAVE_GAMEERROR:
            return GAMEERROR_LEAVETYPE_NAME;
        }

        // ggz_error_msg("ggz_leavetype_to_string: "
        // "invalid leavetype %d given.", type);
        return ""; /* ? */
    }

    public static LeaveType string_to_leavetype(String type_str) {
        if (type_str == null)
            return GGZ_LEAVE_GAMEERROR;

        if (NORMAL_LEAVETYPE_NAME.equalsIgnoreCase(type_str))
            return GGZ_LEAVE_NORMAL;
        else if (BOOT_LEAVETYPE_NAME.equalsIgnoreCase(type_str))
            return GGZ_LEAVE_BOOT;
        else if (GAMEOVER_LEAVETYPE_NAME.equalsIgnoreCase(type_str))
            return GGZ_LEAVE_GAMEOVER;
        else if (GAMEERROR_LEAVETYPE_NAME.equalsIgnoreCase(type_str))
            return GGZ_LEAVE_GAMEERROR;

        return GGZ_LEAVE_GAMEERROR;
    }
}