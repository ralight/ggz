package ggz.client.core;

public interface GameEventListener {
    public void game_launched();

    public void game_launch_fail(Exception e);

    public void game_negotiated();

    public void game_negotiate_fail();

    public void game_playing();

}
