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

import java.io.IOException;
import java.net.Socket;
import java.util.List;

/**
 * This class doesn't actually do any IO. It justs forwards the requests
 * directly between the core client and the game client. It's nice and efficient
 * when the game client is running in the same JVM, which is the common case.
 * 
 * @author Helg.Bredow
 * 
 */
public class EmbeddedIO implements ModGGZIO, ModGameIO {
    private ModGGZ modGGZ;

    private ModGame modGame;

    public EmbeddedIO(ModGGZ modGGZ, ModGame modGame) {
        this.modGGZ = modGGZ;
        this.modGame = modGame;
        this.modGame.setIO(this);
    }
    
    public void disconnect() throws IOException {
        this.modGame.disconnect();
    }

    public void send_error(String error) {
        this.modGame.handleError(error);
    }

    public void send_launch() throws IOException {
        this.modGame.handleLaunch();
    }

    public void send_msg_chat(String player, String chat_msg)
            throws IOException {
        this.modGame.handleChat(player, chat_msg);
    }

    public void send_msg_info(List infos) throws IOException {
        int num = infos.size();

        for (int i = 0; i < num; i++) {
            PlayerInfo info = (PlayerInfo) infos.get(i);

            this.modGame.handlePlayerInfo(info, (num == 1));
        }

        // Only call finished event once instead of for every seat.
        if (num != 1) {
            this.modGame.handlePlayerInfo(null, true);
        }
    }

    public void send_player(String name, boolean is_spectator, int seat_num)
            throws IOException {
        this.modGame.handlePlayer(name, is_spectator, seat_num);
    }

    public void send_seat(Seat seat) throws IOException {
        this.modGame.handleSeat(seat);
    }

    public void send_server(String host, int port, String handle)
            throws IOException {
        this.modGame.handleServer(host, port, handle);
    }

    public void send_server_fd(Socket server_fd) throws IOException {
        this.modGame.handleServerFd(server_fd);
    }

    public void send_spectator_seat(SpectatorSeat seat) throws IOException {
        this.modGame.handleSpectatorSeat(seat);
    }

    public void send_stats(Stat[] player_stats, Stat[] spectator_stats)
            throws IOException {
        this.modGame.handleStats(player_stats, spectator_stats);
    }

    public void sendReqBoot(String name) throws IOException {
        this.modGGZ.handle_boot_request(name);
    }

    public void sendReqInfo(int seat_num) throws IOException {
        this.modGGZ.handle_info_request(seat_num);
    }

    public void sendReqSit(int seat_num) throws IOException {
        this.modGGZ.handle_sit_request(seat_num);
    }

    public void sendReqStand() throws IOException {
        this.modGGZ.handle_stand_request();
    }

    public void sendRequestBot(int seat_num) throws IOException {
        this.modGGZ.handle_bot_request(seat_num);
    }

    public void sendRequestChat(ChatType type, String target, String chat_msg)
            throws IOException {
        this.modGGZ.handle_chat_request(type, target, chat_msg);
    }

    public void sendRequestOpen(int seat_num) throws IOException {
        this.modGGZ.handle_open_request(seat_num);
    }

    public void sendState(ModState state) throws IOException {
        this.modGGZ.handle_state(state);
    }

}
