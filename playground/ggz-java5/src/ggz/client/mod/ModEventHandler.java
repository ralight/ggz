package ggz.client.mod;

import java.io.IOException;
import java.net.Socket;
import java.util.List;

public interface ModEventHandler {
    public void handle_launch() throws IOException;
    public void handle_server_fd(Socket fd) throws IOException;
    public void handle_chat(String player, String msg);
    public void handle_info(int num, List infos);
    public void handle_player(String name, boolean is_spectator, int seat_num);
    public void handle_seat(Seat seat);
    public void handle_spectator_seat(SpectatorSeat seat);
}
