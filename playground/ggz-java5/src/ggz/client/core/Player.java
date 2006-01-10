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
    private int wins, losses, ties, forfeits;

    /* Rating of the player (or NO_RATING) */
    private int rating;

    /* Ranking of the player (or NO_RANKING) */
    private int ranking;

    /* Player's highest score (or NO_HIGHSCORE) */
    private int highscore;

    /* Publicly exported functions */

    public String get_name() {
        return this.name;
    }

    /*
     * int get_record( int *wins, int *losses, int *ties, int *forfeits) { if
     * (!player || !wins || !losses || !ties || !forfeits) return 0; return
     * _get_record(player, wins, losses, ties, forfeits); }
     * 
     * int get_rating( int *rating) { if (!player || !rating) return 0; return
     * _get_rating(player, rating); }
     * 
     * int get_ranking( int *ranking) { if (!player || !ranking) return 0;
     * return _get_ranking(player, ranking); }
     * 
     * int get_highscore( int *highscore) { if (!highscore) return 0; if
     * (this.highscore == NO_HIGHSCORE) { highscore = NO_HIGHSCORE; return 0; }
     * highscore = this.highscore; return 1; }
     */

    /*
     * Internal library functions (prototypes in player.h) NOTE:All of these
     * functions assume valid inputs!
     */

    Player(String name, Room room, int table, PlayerType type, int lag) {

        /* Set to invalid table */
        this.table = -1;

        /* Assume no lag */
        this.lag = -1;

        this.wins = NO_RECORD;
        this.losses = NO_RECORD;
        this.ties = NO_RECORD;
        this.forfeits = NO_RECORD;
        this.rating = NO_RATING;
        this.ranking = NO_RANKING;
        this.highscore = NO_HIGHSCORE;

        // from C init() function
        this.name = name;
        this.room = room;
        this.table = table;
        this.type = type;
        this.lag = lag;
    }

    void init_stats(int wins, int losses, int ties, int forfeits, int rating,
            int ranking, int highscore) {
        this.wins = wins;
        this.losses = losses;
        this.ties = ties;
        this.forfeits = forfeits;
        this.rating = rating;
        this.ranking = ranking;
        this.highscore = highscore;
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

    /*
     * int _get_record( int *wins, int *losses, int *ties, int *forfeits) { if
     * (this.wins == NO_RECORD && this.losses == NO_RECORD && this.ties ==
     * NO_RECORD && this.forfeits == NO_RECORD) { wins = NO_RECORD; losses =
     * NO_RECORD; ties = NO_RECORD; forfeits = NO_RECORD; return 0; } #ifndef
     * MAX # define MAX(a, b) ((a) > (b) ? (a) : (b)) #endif
     *  /* NO_RECORD is -1. If we have a stat for anything, we should return all
     * stats and assume 0 for any we don't know.
     */

    /*
     * wins = MAX(this.wins, 0); losses = MAX(this.losses, 0); ties =
     * MAX(this.ties, 0); forfeits = MAX(this.forfeits, 0);
     * 
     * return 1; }
     * 
     * 
     * int _get_rating( int *rating) { if (this.rating == NO_RATING) { rating =
     * NO_RATING; return 0; } rating = this.rating; return 1; }
     * 
     * 
     * int _get_ranking( int *ranking) { if (this.ranking == NO_RANKING) {
     * ranking = NO_RANKING; return 0; } ranking = this.ranking; return 1; }
     */

    // int _compare( void *p, void *q)
    // {
    // GGZPlayer *s_p = p;
    // GGZPlayer *s_q = q;
    //
    // return strcmp(s_p->name, s_q->name);
    // }
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

    // void *_create(void *p)
    // {
    // GGZPlayer *new, *src = p;
    //
    // new = _new();
    // _init(new, src->name, src->room, src->table,
    // src->type, src->lag);
    //
    // return (void *)new;
    // }

}
