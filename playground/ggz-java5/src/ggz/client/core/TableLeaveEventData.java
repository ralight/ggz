package ggz.client.core;

import ggz.common.LeaveType;

/** @brief The data associated with a GGZ_TABLE_LEFT room event. */
public class TableLeaveEventData {

    /** @brief This is the reason for why we left. */
    private LeaveType reason;

    /**
     * If we were booted from the table (GGZ_LEAVE_BOOT), this is the player who
     * kicked us out.
     */
    private String player;
    
    public TableLeaveEventData(LeaveType reason, String player) {
        this.reason = reason;
        this.player = player;
    }
    
    public LeaveType get_reason() {
        return reason;
    }
    
    public String get_booter() {
        return player;
    }
}
