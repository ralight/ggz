package ggz.client.core;

import ggz.common.PlayerType;

/* 
 * The Player structure is meant to be a node in a list of
 * the players in the current room .
 */
public class Player {
    public static final int NO_RECORD = -1;

    public static final int NO_RATING = 0;

    public static final int NO_RANKING = 0;

    public static final int NO_HIGHSCORE = -1;

    /* Name of player */
    private String name;

    /* Type of player */
    private PlayerType type;

    /* Pointer to room player is in */
    private Room room;

    /* Server ID of table player is at */
    private int table;

    /* Lag of the player */
    private int lag;

    /* Record of the player (or NO_RECORD). */
    public int wins, losses, ties, forfeits;

    /* Rating of the player (or NO_RATING) */
    private int rating;

    /* Ranking of the player (or NO_RANKING) */
    private int ranking;

    /* Player's highest score (or NO_HIGHSCORE) */
    private int highscore;

    public String get_name() {
        return this.name;
    }

    public int get_rating() {
        return this.rating;
    }

    public int get_ranking() {
        return this.ranking;
    }

    public int get_highscore() {
        return this.highscore;
    }

    public int get_wins() {
        return this.wins;
    }

    public int get_losses() {
        return this.losses;
    }

    public int get_ties() {
        return this.ties;
    }

    public int get_forfeits() {
        return this.forfeits;
    }

    /*
     * Internal library functions (prototypes in player.h) NOTE:All of these
     * functions assume valid inputs!
     */

    Player(String name, Room room, int table, PlayerType type, int lag) {
        this.wins = NO_RECORD;
        this.losses = NO_RECORD;
        this.forfeits = NO_RECORD;
        this.ties = NO_RECORD;
        this.rating = NO_RATING;
        this.ranking = NO_RANKING;
        this.highscore = NO_HIGHSCORE;
        this.name = name;
        this.room = room;
        this.table = table;
        this.type = type;
        this.lag = lag;
    }

    void init_stats(int new_wins, int new_losses, int new_ties,
            int new_forfeits, int new_rating, int new_ranking, int new_highscore) {
        this.wins = new_wins;
        this.losses = new_losses;
        this.ties = new_ties;
        this.forfeits = new_forfeits;
        this.rating = new_rating;
        this.ranking = new_ranking;
        this.highscore = new_highscore;
    }

    void set_table(int table) {
        this.table = table;
    }

    void set_lag(int lag) {
        this.lag = lag;
    }

    public PlayerType get_type() {
        return this.type;
    }

    public Room get_room() {
        return this.room;
    }

    public Table get_table() {
        if (this.table == -1)
            return null;

        return this.room.get_table_by_id(this.table);
    }

    public int get_lag() {
        return this.lag;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof Player)
                && this.name.equals(((Player) o).name);
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.name.hashCode();
    }
}
