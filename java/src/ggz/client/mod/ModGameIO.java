package ggz.client.mod;

import ggz.common.ChatType;

import java.io.IOException;

public interface ModGameIO {

    /* Functions for sending IO messages */
    public abstract void sendState(ModState state) throws IOException;

    public abstract void sendReqStand() throws IOException;

    public abstract void sendReqSit(int seatNum) throws IOException;

    public abstract void sendReqBoot(String name) throws IOException;

    public abstract void sendRequestBot(int seatNum) throws IOException;

    public abstract void sendRequestOpen(int seatNum) throws IOException;

    public abstract void sendRequestChat(ChatType type, String target,
            String chatMsg) throws IOException;

    public abstract void sendReqInfo(int seatNum) throws IOException;

}