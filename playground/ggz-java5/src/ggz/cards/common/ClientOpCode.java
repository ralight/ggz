package ggz.cards.common;

/** Messages from client */
public enum ClientOpCode {
    /*
     * Notifies the server of the client's language. Followed by a string
     * including the language.
     */
    MSG_LANGUAGE,

    /* A newgame response, sent in response to a REQ_NEWGAME. No data. */
    RSP_NEWGAME,

    /*
     * An options response, sent in response to a REQ_OPTIONS. It consists of a
     * number n, followed by a list of n integers, each representing an option
     * selection. The number of options n must be what REQ_OPTIONS has
     * requested.
     */
    RSP_OPTIONS,

    /*
     * A play response, sent in response to a REQ_PLAY. It is followed by a card
     * that the client/user wishes to play. Note that although the REQ_PLAY
     * gives a list of valid cards, the client need not conform to this list. In
     * particular, if the client tries to play an invalid card a MSG_BADPLAY
     * with an user-ready error message will be sent.
     */
    RSP_PLAY,

    /*
     * A bid response, sent in response to a REQ_BID. It consists of only an
     * integer giving the chosen bid (see REQ_BID).
     */
    RSP_BID,

    /*
     * A sync request. The server will send out all data again to sync the
     * client.
     */
    REQ_SYNC;
}
