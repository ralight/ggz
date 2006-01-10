package ggz.client.mod;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.Socket;
import java.util.List;

public interface ModGGZ {
    public void set_module(String pwd, String class_name);

    public ModState get_state();

    public void add_mod_listener(ModTransactionHandler handler);

    public void set_player(String name, boolean is_spectator, int seat_num);

    public void connect() throws InstantiationException,
            IllegalAccessException, InvocationTargetException,
            NoSuchMethodException, ClassNotFoundException, IOException;

    public void disconnect();

    public void inform_chat(String player, String msg);

    public void set_seat(Seat seat);

    public void set_spectator_seat(SpectatorSeat seat);

    public void set_info(int num, List infos);

    public void set_server_fd(Socket fd) throws IOException;
}