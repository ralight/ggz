package ggz.common;

/**
 * Administrative actions.
 * 
 * Administrators or to some extent hosts are able to dissolve player problems
 * by several levels of punishment.
 */
public class AdminType {
    /** Player chat is ignored by all others */
    public static AdminType GGZ_ADMIN_GAG = new AdminType();

    /** Reversion of temporary gagging */
    public static AdminType GGZ_ADMIN_UNGAG = new AdminType();

    /** Player is kicked from the server */
    public static AdminType GGZ_ADMIN_KICK = new AdminType();

    /** Player is banned permanently (NOT USED YET) */
    public static AdminType GGZ_ADMIN_BAN = new AdminType();

    /** Invalid admin type */
    public static AdminType GGZ_ADMIN_UNKNOWN = new AdminType();

}
