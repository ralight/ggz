package ggz.cards.common;

/* Global message types */
/*
 * Each MESSAGE_GAME will be followed by one of these opcodes. This represents
 * game-specific data being transmitted to the frontend.
 */
public enum GameMessage {
    /*
     * A simple text message, containing two strings: a "marker" and the
     * "message".
     */
    GAME_MESSAGE_TEXT,

    /*
     * A text message for the player, consisting of a seat # followed by a
     * message string.
     */
    GAME_MESSAGE_PLAYER,

    /*
     * A list of cards for each player. First comes a "marker" string, then (for
     * each player) an integer n plus n cards for that player.
     */
    GAME_MESSAGE_CARDLIST,

    /*
     * Block data that may be game-specific. It is followed by the (string) name
     * of the game, followed by an integer n followed by n bytes of data. It is
     * up to the client frontend to determine what (if anything) to do with this
     * data; it'll be build at the server end by the game module.
     */
    GAME_MESSAGE_GAME,
 }
