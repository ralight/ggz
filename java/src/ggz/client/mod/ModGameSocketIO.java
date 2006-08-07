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

import ggz.common.ChatType;
import ggz.common.PlayerInfo;
import ggz.common.SeatType;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.net.Socket;

public class ModGameSocketIO extends ModIO implements Runnable, ModGameIO {

    private ModGame ggzmod;

    private Socket socket;

    public ModGameSocketIO(ModGame ggzmod) throws IOException {
        int port = Integer.getInteger("GGZSOCKET", 5899).intValue();
        this.socket = new Socket("localhost", port);
        this.ggzmod = ggzmod;
        this.in = new DataInputStream(new BufferedInputStream(socket
                .getInputStream()));
        this.out = new DataOutputStream(socket.getOutputStream());
        new Thread(this).start();
    }

    public void run() {
        try {
            try {
                while (true) {
                    readData();
                }
            } finally {
                this.ggzmod.disconnect();
            }
        } catch (EOFException e) {
            // this.ggzmod.error(e.getMessage());
        } catch (IOException e) {
            // this.ggzmod.error(e.getMessage());
        }
    }

    /* Functions for sending IO messages */
    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendState(ggz.client.mod.ModState)
     */
    public void sendState(ModState state) throws IOException {
        writeInt(MSG_GAME_STATE);
        writeState(state);

        // If we've just sent the GGZMOD_STATE_DONE state then we're finished
        // and don't need the socket anymore.
        if (state == ModState.GGZMOD_STATE_DONE) {
            this.out.flush();
            this.socket.close();
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendReqStand()
     */
    public void sendReqStand() throws IOException {
        writeInt(REQ_STAND);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendReqSit(int)
     */
    public void sendReqSit(int seatNum) throws IOException {
        writeInt(REQ_SIT);
        writeInt(seatNum);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendReqBoot(java.lang.String)
     */
    public void sendReqBoot(String name) throws IOException {
        writeInt(REQ_BOOT);
        writeString(name);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendRequestBot(int)
     */
    public void sendRequestBot(int seatNum) throws IOException {
        writeInt(REQ_BOT);
        writeInt(seatNum);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendRequestOpen(int)
     */
    public void sendRequestOpen(int seatNum) throws IOException {
        writeInt(REQ_OPEN);
        writeInt(seatNum);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendRequestChat(ggz.common.ChatType,
     *      java.lang.String, java.lang.String)
     */
    public void sendRequestChat(ChatType type, String target, String chatMsg)
            throws IOException {
        if (type != ChatType.GGZ_CHAT_TABLE)
            ggzmod.error("Only table chat is supported.");
        writeInt(REQ_CHAT);
        writeString(chatMsg);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGameIO#sendReqInfo(int)
     */
    public void sendReqInfo(int seatNum) throws IOException {
        writeInt(REQ_INFO);
        writeInt(seatNum);
    }

    /* Functions for reading messages */
    private void readData() throws IOException {
        int op = this.in.readInt();

        switch (op) {
        case MSG_GAME_LAUNCH:
            readMsgLaunch();
            break;
        case MSG_GAME_SERVER:
            readMsgServer();
            break;
        case MSG_GAME_SERVER_FD:
            readMsgServerFd();
            break;
        case MSG_GAME_PLAYER:
            readMsgPlayer();
            break;
        case MSG_GAME_SEAT:
            readMsgSeat();
            break;
        case MSG_GAME_SPECTATOR_SEAT:
            readMsgSpectatorSeat();
            break;
        case MSG_GAME_CHAT:
            readMsgChat();
            break;
        case MSG_GAME_STATS:
            readStats();
            break;
        case MSG_GAME_INFO:
            readInfo();
            break;
        default:
            throw new UnsupportedOperationException(
                    "Unrecognised opcode received from ggzmod-ggz: " + op);
        }
    }

    private void readMsgLaunch() throws IOException {
        this.ggzmod.handleLaunch();
    }

    private void readMsgServer() throws IOException {
        String host = readString();
        int port = this.in.readInt();
        String handle = readString();

        this.ggzmod.handleServer(host, port, handle);
    }

    private void readMsgServerFd() {
        throw new UnsupportedOperationException(
                "Java game clients cannot receive an FD.");
    }

    private void readMsgPlayer() throws IOException {
        String name = readString();
        boolean isSpectator = readBoolean();
        int seatNum = this.in.readInt();

        this.ggzmod.handlePlayer(name, isSpectator, seatNum);
    }

    private void readMsgSeat() throws IOException {
        int num = this.in.readInt();
        SeatType type = readSeatType();
        String name = readString();
        Seat seat = new Seat(num, type, name);
        this.ggzmod.handleSeat(seat);
    }

    private void readMsgSpectatorSeat() throws IOException {
        int num = this.in.readInt();
        String name = readString();
        SpectatorSeat seat = new SpectatorSeat(num, name);
        this.ggzmod.handleSpectatorSeat(seat);
    }

    private void readMsgChat() throws IOException {
        String player = readString();
        String chat = readString();
        this.ggzmod.handleChat(player, chat);
    }

    private void readStats() throws IOException {
        int players = this.ggzmod.getNumSeats();
        int spectators = this.ggzmod.getNumSpectatorSeats();
        Stat[] playerStats = new Stat[players];
        Stat[] spectatorStats = new Stat[spectators];

        for (int i = 0; i < players + spectators; i++) {
            Stat stat = new Stat();
            if (i >= players) {
                spectatorStats[i - players] = stat;
            } else {
                playerStats[i] = stat;
            }

            stat.have_record = this.in.readInt();
            stat.have_rating = this.in.readInt();
            stat.have_ranking = this.in.readInt();
            stat.have_highscore = this.in.readInt();
            stat.wins = this.in.readInt();
            stat.losses = this.in.readInt();
            stat.ties = this.in.readInt();
            stat.forfeits = this.in.readInt();
            stat.rating = this.in.readInt();
            stat.ranking = this.in.readInt();
            stat.highscore = this.in.readInt();
        }

        this.ggzmod.handleStats(playerStats, spectatorStats);
    }

    private void readInfo() throws IOException {
        int num = this.in.readInt();

        for (int i = 0; i < num; i++) {
            int seatNum = this.in.readInt();
            String realname = readString();
            String photo = readString();
            String host = readString();

            this.ggzmod.handlePlayerInfo(new PlayerInfo(seatNum, realname,
                    photo, host), (num == 1));
        }

        // only call finished event once instead of for every seat.
        if (num != 1) {
            this.ggzmod.handlePlayerInfo(null, true);
        }
    }
}
