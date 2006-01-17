package ggz.client.mod;

/**
 * @brief Table states
 * 
 * Each table has a current "state" that is tracked by ggzmod. First the table
 * is executed and begins running. Then it receives a launch event from GGZ and
 * begins waiting for players. At some point a game will be started and played
 * at the table, after which it may return to waiting. Eventually the table will
 * probably halt and then the program will exit.
 * 
 * More specifically, the game is in the CREATED state when it is first
 * executed. It moves to the CONNECTED state after GGZ first communicates with
 * it, and to WAITING after the connection is established with the game server.
 * After this, the game server may use ggzmod_set_state to change between
 * WAITING, PLAYING, and DONE states. A WAITING game is considered waiting for
 * players (or whatever), while a PLAYING game is actively being played (this
 * information may be, but currently is not, propogated back to GGZ for display
 * purposes). Once the state is changed to DONE, the table is considered dead
 * and will exit shortly thereafter (ggzmod_loop will stop looping, etc.) (see
 * the kill_on_exit game option).
 * 
 * Each time the game state changes, a GGZMOD_EVENT_STATE event will be
 * propogated to the game server.
 */
public enum ModState {
    /**
     * @brief Initial state. The game starts out in this state. Once the state
     *        is changed it should never be changed back.
     */
    GGZMOD_STATE_CREATED,

    /**
     * @brief Connected state. After the GGZ client and game client get
     *        connected, the game changes into this state automatically. Once
     *        this happens messages may be sent between these two. Once the game
     *        leaves this state it should never be changed back.
     */
    GGZMOD_STATE_CONNECTED,

    /**
     * @brief Waiting state. After the game client and game server are
     *        connected, the client enters the waiting state. The game client
     *        may now call ggzmod_set_state to change between WAITING, PLAYING,
     *        and DONE states.
     */
    GGZMOD_STATE_WAITING,

    /**
     * @brief Playing state. This state is only entered after the game client
     *        changes state to it via ggzmod_set_state. State may be changed
     *        back and forth between WAITING and PLAYING as many times as are
     *        wanted.
     */
    GGZMOD_STATE_PLAYING,

    /**
     * @brief Done state. Once the game client is done running, ggzmod_set_state
     *        should be called to set the state to done. At this point nothing
     *        "new" can happen. The state cannot be changed again after this.
     *        However the game client will not be terminated by the GGZ client;
     *        GGZ just waits for it to exit of its own volition.
     */
    GGZMOD_STATE_DONE
}