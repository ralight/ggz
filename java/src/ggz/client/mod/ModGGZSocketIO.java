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

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Iterator;
import java.util.List;

public class ModGGZSocketIO extends ModIO implements Runnable, ModGGZIO {
    private Socket socket;

    private ModGGZ ggzMod_GGZ;

    public ModGGZSocketIO(ModGGZ ggzMod_GGZ, Socket fd) throws IOException {
        this.socket = fd;
        this.ggzMod_GGZ = ggzMod_GGZ;
        this.in = new DataInputStream(new BufferedInputStream(socket
                .getInputStream()));
        this.out = new DataOutputStream(socket.getOutputStream());
        new Thread(this).start();
    }

    public void run() {
        try {
            try {
                while (true) {
                    read_data();
                }
            } finally {
                this.ggzMod_GGZ.disconnect();
            }
        } catch (Throwable e) {
            this.ggzMod_GGZ.handle_error(e);
        }
    }

    public void disconnect() throws IOException {
        this.socket.close();
    }

    /* Functions for sending IO messages */
    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_launch()
     */
    public void send_launch() throws IOException {
        writeInt(MSG_GAME_LAUNCH);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_server(java.lang.String, int,
     *      java.lang.String)
     */
    public void send_server(String host, int port, String handle)
            throws IOException {
        writeInt(MSG_GAME_SERVER);
        writeString(host);
        writeInt(port);
        writeString(handle);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_server_fd(java.net.Socket)
     */
    public void send_server_fd(Socket server_fd) {
        throw new UnsupportedOperationException(
                "Cannot write server fd since Java doesn't use file descriptors");
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_player(java.lang.String, boolean, int)
     */
    public void send_player(String name, boolean is_spectator, int seat_num)
            throws IOException {
        writeInt(MSG_GAME_PLAYER);
        writeString(name);
        writeBoolean(is_spectator);
        writeInt(seat_num);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_seat(ggz.client.mod.Seat)
     */
    public void send_seat(Seat seat) throws IOException {
        writeInt(MSG_GAME_SEAT);
        writeInt(seat.getNum());
        writeSeatType(seat.getType());
        writeString(seat.getName() == null ? "" : seat.getName());
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_spectator_seat(ggz.client.mod.SpectatorSeat)
     */
    public void send_spectator_seat(SpectatorSeat seat) throws IOException {
        writeInt(MSG_GAME_SPECTATOR_SEAT);
        writeInt(seat.getNum());
        writeString(seat.getName() == null ? "" : seat.getName());
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_msg_chat(java.lang.String,
     *      java.lang.String)
     */
    public void send_msg_chat(String player, String chat_msg)
            throws IOException {
        writeInt(MSG_GAME_CHAT);
        writeString(player);
        writeString(chat_msg);
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_error(java.lang.String)
     */
    public void send_error(String error) {
        // Do nothing, non-embedded games don't support this.
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_stats(ggz.client.mod.Stat[],
     *      ggz.client.mod.Stat[])
     */
    public void send_stats(Stat[] player_stats, Stat[] spectator_stats)
            throws IOException {
        writeInt(MSG_GAME_STATS);

        int num_players = player_stats.length;
        int num_spectators = spectator_stats.length;
        int num_stats = num_players + num_spectators;
        for (int i = 0; i < num_stats; i++) {
            Stat stat;
            if (i >= num_players) {
                stat = spectator_stats[i - num_players];
            } else {
                stat = player_stats[i];
            }

            writeInt(stat.have_record);
            writeInt(stat.have_rating);
            writeInt(stat.have_ranking);
            writeInt(stat.have_highscore);
            writeInt(stat.wins);
            writeInt(stat.losses);
            writeInt(stat.ties);
            writeInt(stat.forfeits);
            writeInt(stat.rating);
            writeInt(stat.ranking);
            writeInt(stat.highscore);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see ggz.client.mod.ModGGZIO#send_msg_info(java.util.List)
     */
    public void send_msg_info(List infos) throws IOException {
        writeInt(MSG_GAME_INFO);
        writeInt(infos.size());

        for (Iterator iter = infos.iterator(); iter.hasNext();) {
            PlayerInfo info = (PlayerInfo) iter.next();
            writeInt(info.getNum());
            writeString(info.getRealName());
            writeString(info.getPhoto());
            writeString(info.getHost());
        }
    }

    /* Functions for reading messages */
    private void read_data() throws IOException {
        int op = readInt();

        switch (op) {
        case MSG_GAME_STATE:
            read_msg_state();
        case REQ_STAND:
            read_req_stand();
        case REQ_SIT:
            read_req_sit();
        case REQ_BOOT:
            read_req_boot();
        case REQ_BOT:
            read_req_bot();
        case REQ_OPEN:
            read_req_open();
        case REQ_CHAT:
            read_req_chat();
        case REQ_INFO:
            read_req_info();
        }
    }

    private void read_msg_state() throws IOException {
        ModState state = readState();
        ggzMod_GGZ.handle_state(state);
    }

    private void read_req_stand() throws IOException {
        ggzMod_GGZ.handle_stand_request();
    }

    private void read_req_sit() throws IOException {
        int seat_num = readInt();
        ggzMod_GGZ.handle_sit_request(seat_num);
    }

    private void read_req_boot() throws IOException {
        String name = readString();
        ggzMod_GGZ.handle_boot_request(name);
    }

    private void read_req_bot() throws IOException {
        int seat_num = readInt();
        ggzMod_GGZ.handle_bot_request(seat_num);
    }

    private void read_req_open() throws IOException {
        int seat_num = readInt();
        ggzMod_GGZ.handle_open_request(seat_num);
    }

    private void read_req_chat() throws IOException {
        String chat_msg = readString();
        // All chat types from non embedded game clients are assumed to be of
        // type GGZ_CHAT_TABLE.
        ggzMod_GGZ.handle_chat_request(ChatType.GGZ_CHAT_TABLE, null, chat_msg);
    }

    private void read_req_info() throws IOException {
        int seat_num = readInt();
        ggzMod_GGZ.handle_info_request(seat_num);
    }
}
