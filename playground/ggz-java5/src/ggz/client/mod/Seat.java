package ggz.client.mod;

import ggz.common.SeatType;

/**
 * @brief A seat at a GGZ game table.
 * 
 * Each seat at the table is tracked like this.
 */
public class Seat {
    /** Seat index; 0..(num_seats-1). */
    int num;

    /** Type of seat. */
    SeatType type;

    /** Name of player occupying seat. */
    String name;

    public Seat(int num, SeatType type, String name) {
        this.num = num;
        this.type = type;
        this.name = name;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof Seat) && this.num == ((Seat) o).num;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.num;
    }
}
