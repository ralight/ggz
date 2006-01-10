package ggz.client.mod;

/* Transactions between ggzmod-ggz and ggzmod-game */
public enum ModTransaction {
    /*
     * Sit down (stop spectatin; join a seat) Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_SIT,

    /*
     * Stand up (leave your seat; become a spectator) Data: NULL
     */
    GGZMOD_TRANSACTION_STAND,

    /*
     * Boot a player Data: player name (const char*)
     */
    GGZMOD_TRANSACTION_BOOT,

    /*
     * Replace a bot/reserved seat with an open one. Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_OPEN,

    /*
     * Put a bot into an open seat Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_BOT,

    /*
     * Information about one or more players Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_INFO,

    /*
     * A chat originating from the game client. Data: message (const char*)
     */
    GGZMOD_TRANSACTION_CHAT;
}
