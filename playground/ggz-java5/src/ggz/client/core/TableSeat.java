package ggz.client.core;

import ggz.common.SeatType;

public class TableSeat {
    /* Seat index */
    public int index;

    /*
     * Type of player in seat. Used for regular seats only; spectator seats just
     * ignore it.
     */
    public SeatType type;

    /*
     * Player's name; or NULL if none. An empty spectator seat will have no
     * name.
     */
    public String name;

    public TableSeat() {
        super();
    }
    
    public TableSeat(int index, SeatType type, String name) {
        this.index = index;
        this.type = type;
        this.name = name;
    }
}
