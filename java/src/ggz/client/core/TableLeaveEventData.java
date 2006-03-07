/*
 * Copyright (C) 2006  Helg Bredow
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
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
