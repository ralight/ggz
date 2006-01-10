package ggz.client.mod;

import java.io.IOException;

public interface ModGame {
    public String get_player();

    public boolean is_spectator();

    public int get_seat_num();

    public Seat get_seat(int num);

    public int get_num_seats();

    public int get_num_spectator_seats();

    public SpectatorSeat get_spectator_seat(int num);

    public ModState get_state();

    public void set_state(ModState state) throws IOException;

    public void request_stand() throws IOException;

    public void request_sit(int seat_num) throws IOException;

    public void request_boot(String name) throws IOException;

    public void request_bot(int seat_num) throws IOException;

    public void request_open(int seat_num) throws IOException;

    public void request_chat(String chat_msg) throws IOException;

    public void player_request_info(int seat_num) throws IOException;

    // public void player_get_info(int seat_num);
    public void add_mod_event_handler(ModEventHandler handler);
}