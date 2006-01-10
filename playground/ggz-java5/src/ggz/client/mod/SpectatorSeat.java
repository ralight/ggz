package ggz.client.mod;

/**
 * @brief A game spectator entry.
 * 
 * Spectators are kept in their own table. A spectator seat is occupied if it
 * has a name, empty if the name is NULL.
 */
public class SpectatorSeat {
    /** Spectator seat index */
    int num;

    /** The spectator's name (NULL => empty) */
    String name;

    public SpectatorSeat(int num, String name) {
        this.num = num;
        this.name = name;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof SpectatorSeat) && this.num == ((SpectatorSeat) o).num;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.num;
    }

}
