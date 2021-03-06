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

import ggz.common.ChatType;

/** @brief The data associated with a GGZ_CHAT_EVENT room event. */
public class ChatEventData {
    /** The type of chat. */
    public ChatType type;

    /** The person who sent the message, or NULL */
    public String sender;

    /** The message itself (in UTF-8), or NULL */
    public String message;
    
    ChatEventData(ChatType type, String sender, String message) {
        this.type = type;
        this.sender = sender;
        this.message = message;
    }
}
