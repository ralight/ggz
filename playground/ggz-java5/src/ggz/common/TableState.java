package ggz.common;

/**
 * @brief Table state values.
 * 
 * GGZ tables take one of the following states. They are used by the GGZ client
 * and GGZ server. This is *not* necessarily the same as the game state kept by
 * libggzdmod
 */
public enum TableState {
    /** < There is some error with the table */
    GGZ_TABLE_ERROR, // = -1,
    /** < Initial created state for the table. */
    GGZ_TABLE_CREATED, // = 0,
    /** < Waiting for enough players to join before playing */
    GGZ_TABLE_WAITING, // = 1,
    /** < Playing a game */
    GGZ_TABLE_PLAYING, // = 2,
    /** < The game session is finished and the table will soon exit */
    GGZ_TABLE_DONE; // = 3

    public static TableState valueOf(int i) {
        switch (i) {
        case -1:
            return GGZ_TABLE_ERROR;
        default:
            return values()[i + 1];
        }
    }
}