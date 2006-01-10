package ggz.client.core;

/** @brief The data associated with a GGZ_MOTD_LOADED server event. */
public class MotdEventData {
    /** MOTD text message */
    public String motd;

    /** URL of a graphical MOTD webpage, or NULL */
    public String url;
    
    public String priority;
}
