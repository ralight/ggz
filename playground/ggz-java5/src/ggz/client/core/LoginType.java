package ggz.client.core;

/**
 * This controls the type of login a user chooses. A different value will
 * require different information to be sent to the server.
 */
public enum LoginType {
    /** Standard login; uname and correct passwd needed. */
    GGZ_LOGIN,

    /** Guest login; only a uname is required. */
    GGZ_LOGIN_GUEST,

    /**
     * New user login; only a uname is required. Password will be assigned by
     * the server (but can be passed along).
     */
    GGZ_LOGIN_NEW
}
