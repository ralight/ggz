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
package ggz.client.mod;

import ggz.client.core.Server;
import ggz.common.ChatType;
import ggz.common.PlayerInfo;
import ggz.common.SeatType;

import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;

/**
 * How a game is launched (incomplete):
 * 
 * 1. ggz client invokes ggzcore to execute the game. The game executable is
 * started. If socketpair() is available then it is used to create a ggz<->game
 * socket connection.
 * 
 * 2. A game launch packet is sent from the GGZ client to the game client.
 * 
 * 3. The game client sets its state to CONNECTED (from created)..
 * 
 * 4. GGZ-client is informed of the game state change.
 * 
 * 5a. set_server informs the game client about where to connect to. A "server"
 * packet is sent GGZ -> game.
 * 
 * 6ab.The GGZ core client (or the game client) creates a socket and connects to
 * GGZ...
 * 
 * 7b. ...GGZ_GAME_NEGOTIATED ggzcore event is triggered...
 * 
 * 8b. ggzcore_set_server_fd is called by the GGZ core client... ...it calls
 * set_server_fd
 * 
 * 9. the game client sets its state to WAITING
 * 
 * 10. GGZ-client is informed of the game state change
 * 
 * 11. GGZ_GAME_PLAYING ggzcore event is triggered
 * 
 * 12. table-join or table-launch packet is sent from ggz CLIENT->SERVER
 * 
 * 13. ggz SERVER handles join/launch packets, sends response
 * 
 * 14. ... and then what? ...
 */
public class ModGame {
    private static final Logger log = Logger.getLogger(ModGame.class.getName());

    private ModState state;

    private Socket serverSocket;

    private ModEventHandler listener;

    private Object gamedata;

    private String myName;

    private boolean iAmSpectator;

    private int mySeatNum;

    private ArrayList seats;

    private ArrayList spectatorSeats;

    private ModGameIO io;

    /** Creates a new ggzmod object. */
    ModGame() {
        // initialize
        this.state = ModState.GGZMOD_STATE_CREATED;
        this.serverSocket = null;
        this.listener = null;
        this.gamedata = null;
        this.mySeatNum = -1;
        this.seats = new ArrayList();
        this.spectatorSeats = new ArrayList();
    }

    void setIO(ModGameIO gameIO) {
        this.io = gameIO;
    }

    /**
     * Get the file descriptor for the GGZMod socket. The ggzmod socket is the
     * main ggz<->game server communications socket.
     * 
     * @param ggzmod
     *            The GGZMod object.
     * @return GGZMod's main ggz <-> table socket FD.
     */
    // public Socket getSocket() {
    // return this.socket;
    // }
    /**
     * Get the current state of the table.
     * 
     * @return The state of the table.
     */
    public ModState getState() {
        return this.state;
    }

    /**
     * Get the socket of the game server connection.
     * 
     * @return The server connection fd
     */
    public Socket getServerSocket() {
        return this.serverSocket;
    }

    /**
     * Return gamedata object.
     * 
     * Each GGZMod object can be given a "gamedata" object that is returned by
     * this function. This is useful for when a single process serves multiple
     * GGZmod's.
     * 
     * @return A the gamedata object (or NULL if none).
     * @see #setGameData(Object)
     */
    public Object getGamedata() {
        return this.gamedata;
    }

    /**
     * Set gamedata object.
     * 
     * @param data
     *            The gamedata object (or NULL for none).
     * @see #getGameData()
     */
    void setGamedata(Object data) {
        this.gamedata = data;
    }

    public void setHandler(ModEventHandler listener) {
        this.listener = listener;
    }

    /**
     * Change the table's state.
     * 
     * This function should be called to change the state of a table. A game can
     * use this function to change state between WAITING and PLAYING, or to set
     * it to DONE.
     * 
     * @param state
     *            The new state.
     */
    public void setState(ModState state) throws IOException {
        // The game may only change the state from one of these two states.
        if (this.state != ModState.GGZMOD_STATE_WAITING
                && this.state != ModState.GGZMOD_STATE_PLAYING)
            throw new IllegalStateException("Cannot change state from "
                    + this.state + "->" + state);

        // The game may only change the state to one of these three states.
        if (state == ModState.GGZMOD_STATE_PLAYING
                || state == ModState.GGZMOD_STATE_WAITING
                || state == ModState.GGZMOD_STATE_DONE)
            _setState(state);
        else
            throw new IllegalStateException("Cannot change state from "
                    + this.state + "->" + state);
    }

    private void setPlayer(String name, boolean isSpectator, int seatNum) {
        this.myName = name;
        this.iAmSpectator = isSpectator;
        this.mySeatNum = seatNum;
    }

    public void handlePlayer(String name, boolean isSpectator, int seatNum) {
        String oldName = this.myName;
        boolean oldIsSpectator = this.iAmSpectator;
        int oldSeatNum = this.mySeatNum;

        setPlayer(name, isSpectator, seatNum);

        if (this.state != ModState.GGZMOD_STATE_CREATED)
            if (listener != null)
                listener.handlePlayer(oldName, oldIsSpectator, oldSeatNum);
    }

    /**
     * Gets the name of the player that we represent.
     * 
     * @return The name of the player.
     */
    public String getMyName() {
        if (this.state == ModState.GGZMOD_STATE_CREATED)
            throw new IllegalStateException(
                    "Can't call getMyName() when state is CREATED.");
        return this.myName;
    }

    /**
     * 
     * @return true iff player is spectating.
     */
    public boolean isSpectator() {
        if (this.state == ModState.GGZMOD_STATE_CREATED)
            throw new IllegalStateException(
                    "Can't call getMyName() when state is CREATED.");
        return this.iAmSpectator;
    }

    /**
     * Call this function to find out where at the table this player is sitting.
     * 
     * @return the number of our (spectator) seat.
     */
    public int getMySeatNum() {
        if (this.state == ModState.GGZMOD_STATE_CREATED)
            throw new IllegalStateException(
                    "Can't call getMyName() when state is CREATED.");
        return this.mySeatNum;
    }

    /**
     * Get the total number of seats at the table. If no connection is present,
     * 0 will be returned. While in GGZMOD_STATE_CREATED, we don't know the
     * number of seats.
     * 
     * @return The number of seats.
     */
    public int getNumSeats() {
        // Note: num_seats is initialized to 0 and isn't changed until we hear
        // differently from GGZ.
        return this.seats.size();
    }

    /**
     * Get the maximum number of spectators. This function returns the maximum
     * number of spectator seats available. A game can use this to iterate over
     * the spectator seats to look for spectators occupying them. Since
     * spectators may come and go at any point and there is no limit on the
     * number of spectators, you should consider this value to be dynamic and
     * call this function again each time you're looking for spectators.
     * 
     * @return The number of available spectator seats.
     */
    public int getNumSpectatorSeats() {
        // Note: num_spectator_seats is initialized to 0 and isn't changed until
        // we hear differently from GGZ.
        return this.spectatorSeats.size();
    }

    /**
     * Get all data for the specified seat.
     * 
     * @param seatNum
     *            The seat number (0..(number of seats - 1)).
     * @return A valid Seat structure, if seat is a valid seat.
     */
    public Seat getSeat(int seatNum) {
        int numSeats = this.seats.size();
        for (int i = 0; i < numSeats; i++) {
            Seat seat = (Seat) this.seats.get(i);
            if (seat.getNum() == seatNum)
                return seat;
        }
        return null;
    }

    /**
     * Get a spectator's data.
     * 
     * @param seatNum
     *            The number, between 0 and (number of spectators - 1).
     * @return A valid SpectatorSeat object, if given a valid seat.
     */
    public SpectatorSeat getSpectatorSeat(int seatNum) {
        int numSeats = this.spectatorSeats.size();
        for (int i = 0; i < numSeats; i++) {
            SpectatorSeat seat = (SpectatorSeat) this.spectatorSeats.get(i);
            if (seat.getNum() == seatNum)
                return seat;
        }
        return null;
    }

    private void setSeat(Seat seat) {
        this.seats.add(seat);
    }

    void handleSeat(Seat seat) {
        Seat oldSeat = getSeat(seat.getNum());

        // Remove the old seat.
        if (oldSeat != null)
            this.seats.remove(oldSeat);

        // Place the new seat into the list.
        setSeat(seat);

        // Invoke the handler.
        if (this.state != ModState.GGZMOD_STATE_CREATED)
            if (listener != null)
                listener.handleSeat(oldSeat);
    }

    private void setSpectatorSeat(SpectatorSeat seat) {
        if (seat.getName() != null) {
            this.spectatorSeats.add(seat);
        } else {
            Object oldSeat = getSpectatorSeat(seat.getNum());
            if (oldSeat != null)
                this.spectatorSeats.remove(oldSeat);
        }
    }

    void handleSpectatorSeat(SpectatorSeat seat) {
        SpectatorSeat oldSeat = getSpectatorSeat(seat.getNum());

        // Remove the old seat.
        if (oldSeat != null)
            this.spectatorSeats.remove(oldSeat);

        // Place the new seat into the list.
        setSpectatorSeat(seat);

        // Invoke the handler.
        if (this.state != ModState.GGZMOD_STATE_CREATED)
            if (listener != null)
                listener.handleSpectatorSeat(oldSeat, seat);
    }

    void handleChat(String player, String chatMsg) {
        if (listener != null)
            listener.handleChat(player, chatMsg);
    }

    void handleError(String msg) {
        if (listener != null)
            listener.handleError(msg);
    }

    void handleStats(Stat[] playerStats, Stat[] spectatorStats) {
        for (int i = 0; i < playerStats.length; i++) {
            Seat seat = getSeat(playerStats[i].number);
            if (seat != null)
                seat.setStat(playerStats[i]);
        }

        for (int i = 0; i < spectatorStats.length; i++) {
            SpectatorSeat seat = getSpectatorSeat(spectatorStats[i].number);
            if (seat != null)
                seat.setStat(spectatorStats[i]);
        }
        if (listener != null)
            listener.handleStats();
    }

    void handlePlayerInfo(PlayerInfo info, boolean finish) {
        // Find the player for whom this info is for.
        for (Iterator iter = seats.iterator(); iter.hasNext();) {
            Seat seat = (Seat) iter.next();
            if (seat.getNum() == info.getNum()) {
                seat.setInfo(info);
                break;
            }
        }

        if (listener != null) {
            listener.handleInfo(info);
        }
    }

    /**
     * Connect to ggz.
     * 
     * Call this function to make an initial GGZ core client <-> game client
     * connection.
     */
    public void connect() throws IOException {
        if (io == null) {
            // We haven't been passed an embedded IO so assume we are running
            // externally and so need to connect to the core client via a
            // socket.
            this.io = new ModGameSocketIO(this);
        }
    }

    /**
     * Disconnect from ggz.
     * 
     * This terminates the link between the game client and the GGZ core client.
     */
    public void disconnect() throws IOException {
        if (this.io == null) {
            // This isn't an error; it usually means we already disconnected.
            return;
        }

        // First warn the server of halt (if we haven't already) */
        _setState(ModState.GGZMOD_STATE_DONE);
        log.fine("Disconnected from GGZ server.");
        this.io = null;
    }

    private void _setState(ModState newState) throws IOException {
        ModState oldState = this.state;
        if (newState == this.state)
            return; /* Is this an error? */

        // The callback function retrieves the state from getState().
        // It could instead be passed as an argument.
        this.state = newState;
        if (listener != null)
            listener.handleState(oldState);

        log.fine("Game setting state to " + newState);
        this.io.sendState(newState);
    }

    /** ** Transaction requests *** */

    /**
     * Stand up (move from your seat into a spectator seat).
     */
    public void requestStand() throws IOException {
        this.io.sendReqStand();
    }

    /**
     * Sit down (move from a spectator seat into a player seat).
     * 
     * @param seatNum
     *            The seat to sit in.
     */
    public void requestSit(int seatNum) throws IOException {
        this.io.sendReqSit(seatNum);
    }

    /**
     * Boot a player. Only the game host may do this.
     * 
     * @param name
     *            The name of the player to boot.
     */
    public void requestBoot(String name) throws IOException {
        this.io.sendReqBoot(name);
    }

    /**
     * Change the requested seat from an open seat to a bot.
     * 
     * @param seatNum
     *            The number of the seat to toggle.
     */
    public void requestBot(int seatNum) throws IOException {
        this.io.sendRequestBot(seatNum);
    }

    /**
     * Change the requested seat from a bot to an open seat.
     * 
     * @param seatNum
     *            The number of the seat to toggle.
     */
    public void requestOpen(int seatNum) throws IOException {
        this.io.sendRequestOpen(seatNum);
    }

    /**
     * Chat! This initiates a table chat.
     * 
     * @param chatMsg
     *            The chat message.
     */
    public void requestChat(ChatType type, String target, String chatMsg)
            throws IOException {
        this.io.sendRequestChat(type, target, chatMsg);
    }

    /** ** Internal library functions *** */

    void error(String error) {
        if (listener != null)
            listener.handleError(error);
    }

    void handleLaunch() throws IOException {
        // Normally we let the game control its own state, but we control the
        // transition from CREATED to CONNECTED.
        _setState(ModState.GGZMOD_STATE_CONNECTED);
    }

    void handleServer(String host, int port, String handle) throws IOException {
        if (this.serverSocket == null) {
            // In the case of game clients connecting themselves, do it here.
            this.serverSocket = Server.channel_connect(host, port, handle);
            _setState(ModState.GGZMOD_STATE_WAITING);
            if (listener != null)
                listener.handleServer(serverSocket);
        }
    }

    void handleServerFd(Socket fd) throws IOException {
        this.serverSocket = fd;

        // Everything's done now, so we move into the waiting state.
        _setState(ModState.GGZMOD_STATE_WAITING);
        if (listener != null)
            listener.handleServer(serverSocket);
    }

    /*
     * int player_get_record(, GGZSeat *seat, int *wins, int *losses, int *ties,
     * int *forfeits) { GGZStat search_stat = {.number = seat->num};
     * GGZListEntry *entry = ggz_list_search(this.stats, &search_stat); GGZStat
     * *stat = ggz_list_get_data(entry);
     * 
     * if (!stat || !stat->have_record) return 0;
     * 
     * wins = stat->wins; losses = stat->losses; ties = stat->ties; forfeits =
     * stat->forfeits; return 1; }
     * 
     * int player_get_rating(, GGZSeat *seat, int *rating) { GGZStat search_stat =
     * {.number = seat->num}; GGZListEntry *entry = ggz_list_search(this.stats,
     * &search_stat); GGZStat *stat = ggz_list_get_data(entry);
     * 
     * if (!stat || !stat->have_rating) return 0;
     * 
     * rating = stat->rating; return 1; }
     * 
     * int player_get_ranking(, GGZSeat *seat, int *ranking) { GGZStat
     * search_stat = {.number = seat->num}; GGZListEntry *entry =
     * ggz_list_search(this.stats, &search_stat); GGZStat *stat =
     * ggz_list_get_data(entry);
     * 
     * if (!stat || !stat->have_ranking) return 0;
     * 
     * ranking = stat->ranking; return 1; }
     * 
     * int player_get_highscore(, GGZSeat *seat, int *highscore) { GGZStat
     * search_stat = {.number = seat->num}; GGZListEntry *entry =
     * ggz_list_search(this.stats, &search_stat); GGZStat *stat =
     * ggz_list_get_data(entry);
     * 
     * if (!stat || !stat->have_highscore) return 0;
     * 
     * highscore = stat->highscore; return 1; }
     * 
     * int spectator_get_record(, GGZSpectatorSeat *seat, int *wins, int
     * *losses, int *ties, int *forfeits) { GGZStat search_stat = {.number =
     * seat->num}; GGZListEntry *entry = ggz_list_search(this.spectator_stats,
     * &search_stat); GGZStat *stat = ggz_list_get_data(entry);
     * 
     * if(!stat) return 0; wins = stat->wins; losses = stat->losses; ties =
     * stat->ties; forfeits = stat->forfeits; return 1; }
     * 
     * int spectator_get_rating(, GGZSpectatorSeat *seat, int *rating) { GGZStat
     * search_stat = {.number = seat->num}; GGZListEntry *entry =
     * ggz_list_search(this.spectator_stats, &search_stat); GGZStat *stat =
     * ggz_list_get_data(entry);
     * 
     * if(!stat) return 0; rating = stat->rating; return 1; }
     * 
     * int spectator_get_ranking(, GGZSpectatorSeat *seat, int *ranking) {
     * GGZStat search_stat = {.number = seat->num}; GGZListEntry *entry =
     * ggz_list_search(this.spectator_stats, &search_stat); GGZStat *stat =
     * ggz_list_get_data(entry);
     * 
     * if(!stat) return 0; ranking = stat->ranking; return 1; }
     * 
     * int spectator_get_highscore(, GGZSpectatorSeat *seat, int *highscore) {
     * GGZStat search_stat = {.number = seat->num}; GGZListEntry *entry =
     * ggz_list_search(this.spectator_stats, &search_stat); GGZStat *stat =
     * ggz_list_get_data(entry);
     * 
     * if(!stat) return 0; highscore = stat->highscore; return 1; }
     */

    /**
     * Request extended player information for one or more players
     * 
     * Depending on the seat parameter (-1 or valid number), this function
     * asynchronously requests information about player(s), which trigget
     * ModEventHandler.handleInfo().
     * 
     * @param seatNum
     *            The seat number to request info for, or -1 to select all.
     */
    public void requestPlayerInfo(int seatNum) throws IOException {
        if (seatNum != -1) {
            Seat seat = getSeat(seatNum);
            if (seat == null)
                throw new IllegalArgumentException(
                        "There is no seat with that number: " + seatNum);
            if (seat.getType() != SeatType.GGZ_SEAT_PLAYER)
                throw new IllegalArgumentException(
                        "Cannot request information for seat " + seatNum
                                + " because it is of the wrong type: "
                                + seat.getType());
        }
        this.io.sendReqInfo(seatNum);
    }

    /*
     * PlayerInfo getPlayerInfo(int seat) { GGZPlayerInfo search_info = {.num =
     * seat}; GGZListEntry *entry = ggz_list_search(this.infos, &search_info);
     * 
     * return entry ? ggz_list_get_data(entry) : null; }
     */
}
