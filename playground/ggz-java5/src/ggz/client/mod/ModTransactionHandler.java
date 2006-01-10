package ggz.client.mod;

import java.io.IOException;
import java.util.EventListener;

public interface ModTransactionHandler extends EventListener {
    public void handle_state(ModState prev) throws IOException;

    public void handle_sit(int seat_num) throws IOException;

    public void handle_stand() throws IOException;

    public void handle_boot(String name) throws IOException;

    public void handle_seatchange(ModTransaction t, int seat_num) throws IOException;

    public void handle_chat(String chat) throws IOException;

    public void handle_info(int seat_num) throws IOException;

}
