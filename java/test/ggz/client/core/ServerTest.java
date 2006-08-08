package ggz.client.core;

import java.io.IOException;

public class ServerTest implements ServerListener {
    private Server server;

    /**
     * @param args
     */
    public static void main(String[] args) throws IOException {
        Server server = new Server("oojah.dyndns.org", 5688, false);
        server.add_event_hook(new ServerTest(server));
        server.logout();
        server.connect();
    }

    public ServerTest(Server s) {
        server = s;
    }

    public void server_channel_connected() {
        // TODO Auto-generated method stub
    }

    public void server_channel_fail(String error) {
        // TODO Auto-generated method stub

    }

    public void server_channel_ready() {
        // TODO Auto-generated method stub

    }

    public void server_chat_fail(ErrorEventData data) {
        // TODO Auto-generated method stub

    }

    public void server_connect_fail(String error) {
        System.err.println("Connect Fail: " + error);
    }

    public void server_connected() {
        System.out.println("Server connected");
    }

    public void server_enter_fail(ErrorEventData data) {
        System.err.println("Enter fail: " + data.message);
    }

    public void server_enter_ok() {
        System.out.println("Enter OK");
    }

    public void server_list_rooms() {
        System.out.println("Server rooms listed OK");
    }

    public void server_list_types() {
        // TODO Auto-generated method stub

    }

    public void server_logged_out() {
        System.out.println("Logged out.");
    }

    public void server_login_fail(ErrorEventData data) {
        System.err.println("Login Failed: " + data.message);
        try {
            server.logout();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void server_login_ok() {
        System.out.println("Login OK");
        try {
            System.out.println("Listing rooms");
            server.list_rooms(false);
            System.out.println("Joining room 0");
            server.join_room(0);
            System.out.println("Joining room 1");
            server.join_room(1);
            System.out.println("Logging out");
            server.logout();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void server_motd_loaded(MotdEventData data) {
        // TODO Auto-generated method stub

    }

    public void server_negotiate_fail(String error) {
        System.err.println("Negotiate fail: " + error);
    }

    public void server_negotiated() {
        try {
            System.out.println("Logging in");
            server.set_logininfo(LoginType.GGZ_LOGIN_GUEST, "ServerTest",
                    null, null);
            server.login();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void server_net_error(String error) {
        System.err.println("Net error: " + error);
    }

    public void server_players_changed() {
        // TODO Auto-generated method stub

    }

    public void server_rooms_changed() {
        // TODO Auto-generated method stub

    }

    public void server_protocol_error(String error) {
        // TODO Auto-generated method stub

    }

    public void server_state_changed() {
        System.out.println("Server state changed: " + server.get_state());
    }

}
