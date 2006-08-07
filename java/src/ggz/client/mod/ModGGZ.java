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

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;
import java.util.logging.Logger;

public class ModGGZ {
    private static final Logger log = Logger.getLogger(ModGGZ.class.getName());

    private ModState state;

    private Socket server_fd;

    private String server_host;

    private int server_port;

    private String server_handle;

    private ModTransactionHandler thandler;

    // private String my_name;
    //
    // private boolean i_am_spectator;
    //
    // private int my_seat_num;

    // private ArrayList seats;
    // private ArrayList spectator_seats;
    // private ArrayList stats;
    // private ArrayList infos;
    private String pwd;

    private String cmd;

    private String className;

    private Process process;

    private ModGGZIO io;

    /** Creates a new ggzmod object. */
    public ModGGZ() {
        // initialize
        this.state = ModState.GGZMOD_STATE_CREATED;
    }

    public ModState get_state() {
        return this.state;
    }

    public void set_module(String className) {
        this.className = className;
    }

    public void set_module(String pwd, String cmd) {
        log.fine("Setting arguments");
        log.fine("cmd=" + cmd);
        log.fine("pwd=" + pwd);

        // Check parameters.
        if (cmd == null) {
            throw new IllegalArgumentException("Bad module arguments");
        }

        this.cmd = cmd;
        this.pwd = pwd;
    }

    public void set_server_host(String host, int port, String handle) {
        // If we're already connected and not embedded, send the socket.
        if (this.state == ModState.GGZMOD_STATE_CONNECTED
                && this.className == null) {
            try {
                this.io.send_server(host, port, handle);
            } catch (IOException ex) {
                handle_error(ex);
            }
        }
        this.server_host = host;
        this.server_port = port;
        this.server_handle = handle;
    }

    public void set_server_fd(Socket fd) {
        /* If we're already connected, send the fd */
        if (this.state == ModState.GGZMOD_STATE_CONNECTED) {
            try {
                this.io.send_server_fd(fd);
            } catch (IOException ex) {
                handle_error(ex);
            }
        }
    }

    // void set_handler(GGZModEvent e,
    // GGZModHandler func)
    // {
    // if (!ggzmod || e < 0 || e >= GGZMOD_NUM_HANDLERS) {
    // ggz_error_msg("set_handler: "
    // "invalid params");
    // return; /* not very useful */
    // }
    //
    // this.handlers[e] = func;
    // }

    public void set_transaction_handler(ModTransactionHandler func) {
        this.thandler = func;
    }

    public void set_player(String name, boolean is_spectator, int seat_num) {

        // this.my_name = name;
        // this.i_am_spectator = is_spectator;
        // this.my_seat_num = seat_num;
        if (this.state != ModState.GGZMOD_STATE_CREATED) {
            try {
                this.io.send_player(name, is_spectator, seat_num);
            } catch (IOException ex) {
                handle_error(ex);
            }
        }

    }

    // private Seat _get_seat( int num)
    // {
    // for (Iterator iter = this.seats.iterator(); iter.hasNext();){
    // Seat seat = (Seat)iter.next();
    // if (seat.get_num() == num)
    // return seat;
    // }
    // return null;
    // }

    public void set_seat(Seat seat) {
        // Seat oldseat;
        //
        // if (seat == null || seat.get_num() < 0) {
        // throw new IllegalArgumentException("Seat is null or seat.getNum() <
        // 0");
        // }
        //
        // // If there is no such seat, return error.
        // oldseat = _get_seat(seat.get_num());
        //
        // if (seat.equals(oldseat)){
        // // No change.
        // return;}

        if (this.state != ModState.GGZMOD_STATE_CREATED) {
            try {
                this.io.send_seat(seat);
            } catch (IOException ex) {
                handle_error(ex);
            }
        }
        // if (oldseat != null)
        // seats.remove(oldseat);
        // seats.add(seat);
    }

    // private void _set_spectator_seat(SpectatorSeat seat)
    // {
    // if (seat.getName()!= null) {
    // spectator_seats.add(seat);
    // } else {
    // /* Non-occupied seats are just empty entries in the list. */
    // GGZListEntry *entry = ggz_list_search(this.spectator_seats,
    // seat);
    // ggz_list_delete_entry(this.spectator_seats, entry);
    //
    // /* FIXME: reduce num_spectator_seats */
    // }
    // }

    // static SpectatorSeat _get_spectator_seat(, int num)
    // {
    // SpectatorSeat seat = {num: num, name: NULL};
    //
    // if (num >= 0 && num < this.num_spectator_seats) {
    // GGZListEntry *entry;
    // entry = ggz_list_search(this.spectator_seats, &seat);
    // if (entry)
    // seat = *(SpectatorSeat*)ggz_list_get_data(entry);
    // }
    //
    // return seat;
    // }

    public void set_spectator_seat(SpectatorSeat seat) {
        if (seat == null)
            throw new IllegalArgumentException("seat cannot be null");
        if (seat.getNum() < 0)
            throw new IllegalArgumentException("seat num cannot be less than 0");

        if (this.state != ModState.GGZMOD_STATE_CREATED) {
            // SpectatorSeat old_seat;
            // old_seat = _get_spectator_seat(ggzmod, seat->num);
            // if (ggz_strcmp(seat->name, old_seat.name)
            // &&
            try {
                this.io.send_spectator_seat(seat);
            } catch (IOException ex) {
                handle_error(ex);
            }
        }
        //
        // _set_spectator_seat(ggzmod, seat);
        //
        // return 0;
    }

    public void inform_chat(String player, String msg) {
        try {
            this.io.send_msg_chat(player, msg);
        } catch (IOException ex) {
            handle_error(ex);
        }
    }

    public void inform_error(String error) {
        this.io.send_error(error);
    }

    /*
     * GGZmod actions
     */

    public void connect() throws IOException {
        /* For the ggz side, we fork the game and then send the launch message */

        if (this.cmd != null) {
            game_fork();
        } else {
            log.fine("Running embedded game (no fork)");
            game_embedded();
        }

        send_game_launch();
    }

    public void disconnect() throws IOException {
        if (this.io == null) {
            /*
             * This isn't an error; it usually means we already disconnected.
             * The invariant is that the process (this.pid) exists iff the
             * socket (this.fd) exists.
             */
            return;
        }

        // Make sure game client is dead.
        if (this.process != null) {
            this.process.destroy();
        }

        set_state(ModState.GGZMOD_STATE_DONE);
        this.io.disconnect();
        this.io = null;
    }

    private void set_state(ModState new_state) throws IOException {
        ModState old_state = this.state;
        if (new_state == this.state)
            return; /* Is this an error? */

        /*
         * The callback function retrieves the state from get_state. It could
         * instead be passed as an argument.
         */
        this.state = new_state;
        if (this.thandler != null)
            thandler.handle_state(old_state);
    }

    /*
     * ggz specific actions
     */

    /** Sends a game launch packet to ggzmod-game. */
    private void send_game_launch() throws IOException {

        // this.io.send_player(this.fd,
        // this.my_name,
        // this.i_am_spectator,
        // this.my_seat_num);
        //
        // for (entry = ggz_list_head(this.seats);
        // entry;
        // entry = ggz_list_next(entry)) {
        // Seat *seat = ggz_list_get_data(entry);
        // if (this.io.send_seat(seat) < 0)
        // return -3;
        // }
        // for (entry = ggz_list_head(this.spectator_seats);
        // entry;
        // entry = ggz_list_next(entry)) {
        // SpectatorSeat *seat = ggz_list_get_data(entry);
        // if (this.io.send_spectator_seat(seat) < 0)
        // return -4;
        // }
        // HB Not sure if there is any sense in sending the above since it's all
        // going to be uninitialised anyway.
        this.io.send_launch();

        // If the server fd has already been set, send that too.
        if (this.server_fd != null)
            this.io.send_server_fd(this.server_fd);

        if (this.server_host != null && this.className == null)
            this.io.send_server(this.server_host, this.server_port,
                    this.server_handle);
    }

    /** Common setup for normal mode and embedded mode */
    // public ServerSocket game_prepare()
    // {
    // ServerSocket sock = new ServerSocket(0);
    // sock.bind(null);
    // if (listen(*sock, 1) < 0) {
    // ggz_error_msg("Could not listen on socket.");
    // return -1;
    // }
    // snprintf(buf, sizeof(buf), "%d", port);
    // #ifdef HAVE_SETENV
    // setenv("GGZSOCKET", buf, 1);
    // setenv("GGZMODE", "true", 1);
    // #else
    // SetEnvironmentVariable("GGZSOCKET", buf);
    // SetEnvironmentVariable("GGZMODE", "true");
    // #endif
    // #endif
    //
    // return 0;
    // }
    /** Forks the game. A negative return value indicates a serious error. */
    private void game_fork() throws IOException {

        // If there are no args, we don't know what to run!
        if (this.cmd == null) {
            throw new IllegalStateException(
                    "Attempt to start process without a command line.");
        }

        // FIXME: this is insecure; it should be restricted to local
        // connections.
        ServerSocket sock = new ServerSocket(0);
        // sock.bind(null);

        this.process = Runtime.getRuntime().exec(
                this.cmd,
                new String[] { "GGZSOCKET=" + sock.getLocalPort(),
                        "GGZMODE=true" }, new File(this.pwd));

        // any error message?
        StreamGobbler errorGobbler = new StreamGobbler(this.process
                .getErrorStream(), "ERROR");

        // any output?
        StreamGobbler outputGobbler = new StreamGobbler(this.process
                .getInputStream(), "OUTPUT");

        // kick them off
        errorGobbler.start();
        outputGobbler.start();

        log.fine("GGZ Core Client listening for game connection on port "
                + sock.getLocalPort());

        // FIXME: we need to select, with a maximum timeout.
        Socket fd = sock.accept();
        log.fine("Game client connected, not accepting anymore connections.");
        sock.close();
        this.io = new ModGGZSocketIO(this, fd);
    }

    /** Similar to game_fork(), but runs the game embedded */
    private void game_embedded() {
        if (this.className == null) {
            throw new IllegalStateException(
                    "Attempt to start game without a class name.");
        }

        try {
            Class c = Class.forName(this.className);
            ModEventHandler game = (ModEventHandler) c.newInstance();
            ModGame modGame = new ModGame();
            this.io = new EmbeddedIO(this, modGame);
            game.init(modGame);
        } catch (Throwable ex) {
            handle_error(ex);
        }
    }

    void handle_state(ModState new_state) throws IOException {
        /*
         * In contradiction to what I say above, the game actually _is_ allowed
         * to change its state from CREATED to WAITING. When ggzmod-ggz sends a
         * launch packet to ggzmod-game, ggzmod-game automatically changes the
         * state from CREATED to WAITING. When this happens, it tells ggzmod-ggz
         * of this change and we end up back here. So, although it's a bit
         * unsafe, we have to allow this for now. The alternative would be to
         * have ggzmod-ggz and ggzmod-game both separately change states when
         * the launch packet is sent.
         */
        set_state(new_state);
    }

    void handle_stand_request() throws IOException {
        if (thandler != null)
            thandler.handle_stand();
    }

    void handle_sit_request(int seat_num) throws IOException {
        if (thandler != null)
            thandler.handle_sit(seat_num);
    }

    void handle_boot_request(String name) throws IOException {
        if (thandler != null)
            thandler.handle_boot(name);
    }

    void handle_bot_request(int seat_num) throws IOException {
        if (thandler != null)
            thandler.handle_bot(seat_num);
    }

    void handle_open_request(int seat_num) throws IOException {
        if (thandler != null)
            thandler.handle_open(seat_num);
    }

    void handle_chat_request(ChatType type, String recipient, String chat_msg)
            throws IOException {
        if (thandler != null)
            thandler.handle_chat(type, recipient, chat_msg);
    }

    void handle_info_request(int seat_num) throws IOException {
        if (thandler != null)
            thandler.handle_info(seat_num);
    }

    public void set_stats(Stat[] player_stats, Stat[] spectator_stats) {
        if (this.state == ModState.GGZMOD_STATE_CREATED)
            return;

        try {
            this.io.send_stats(player_stats, spectator_stats);
        } catch (IOException ex) {
            handle_error(ex);
        }
    }

    public void set_info(List infos) {
        try {
            this.io.send_msg_info(infos);
        } catch (IOException ex) {
            handle_error(ex);
        }
    }

    public void handle_error(Throwable exception) {
        // FIXME: should be disconnect?
        try {
            disconnect();
            // set_state(ModState.GGZMOD_STATE_DONE);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        if (thandler != null)
            thandler.handle_error(exception);
    }
}

class StreamGobbler extends Thread {
    InputStream is;

    String type;

    OutputStream os;

    StreamGobbler(InputStream is, String type) {
        this(is, type, null);
    }

    StreamGobbler(InputStream is, String type, OutputStream redirect) {
        this.is = is;
        this.type = type;
        this.os = redirect;
    }

    public void run() {
        try {
            PrintWriter pw = null;
            if (os != null)
                pw = new PrintWriter(os);

            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(isr);
            String line = null;
            while ((line = br.readLine()) != null) {
                if (pw != null)
                    pw.println(line);
                System.out.println(type + ">" + line);
            }
            if (pw != null)
                pw.flush();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }
}
