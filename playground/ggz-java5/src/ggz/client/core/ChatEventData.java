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
