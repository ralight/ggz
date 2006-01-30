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
package ggz.client.core;

import ggz.common.ChatType;
import ggz.common.ClientReqError;
import ggz.common.LeaveType;
import ggz.common.NumberList;
import ggz.common.PlayerType;
import ggz.common.SeatType;
import ggz.common.TableState;
import ggz.common.XMLElement;

import java.io.FileOutputStream;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Stack;
import java.util.logging.Logger;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

public class Net implements Runnable {
    protected static final Logger log = Logger.getLogger(Net.class.getName());

    /*
     * I thought of putting this into ggz_common (in libggz) along with the
     * protocol opcodes, but it really needs to stay tied to the network code
     * itself (in ggzd and ggzcore).
     */
    public static final int GGZ_CS_PROTO_VERSION = 10;

    /* Server structure handling this session */
    protected Server server;

    /* Host name of server */
    private String host;

    /* Port on which GGZ server in running */
    private int port;

    /* File descriptor for communication with server */
    private Socket fd; // TODO consider making fd a writer instead of socket.

    private OutputStreamWriter out;

    /* Maximum chat size allowed */
    private int chat_size;

    /* XML Parser */
    // XML_Parser parser;
    /* Message parsing stack */
    protected Stack<XMLElement> stack;

    /*
     * Flag to indicate that we've received the closing <SESSION> tag and no
     * more XML is coming.
     */
    boolean is_session_over = false;

    /* Files to dump protocol session */
    private Writer send_dump_file;

    private Writer receive_dump_file;

    /* Whether to use TLS or not */
    private boolean use_tls;

    /* Game data structure */
    private static class GameData {
        protected String prot_engine;

        protected String prot_version;

        protected NumberList player_allow_list;

        protected NumberList bot_allow_list;

        protected boolean spectators_allow;

        protected boolean peers_allow;

        protected String desc;

        protected String author;

        protected String url;

        protected String[][] named_bots;
    }

    /* Player information structure */
    private static class PlayerInfo {
        int num;

        String realname;

        String photo;

        String host;
    }

    private static class PlayerInfoData {
        List<PlayerInfo> infos;
    }

    /* Table data structure */
    private static class TableData {
        protected String desc;

        protected List<TableSeat> seats;

        protected List<TableSeat> spectatorseats;
    }

    private class XML_Parser extends DefaultHandler {

        public void startElement(String uri, String localName, String qName,
                Attributes attributes) {
            /* Create new element object */
            XMLElement element = new XMLElement(localName, attributes);

            /* Put element on stack so we can process its children */
            stack.push(element);

            dump_receive_data("<" + localName);
            for (int i = 0; i < attributes.getLength(); i++) {
                String name = attributes.getQName(i);
                dump_receive_data(" " + name + "='" + attributes.getValue(name)
                        + "'");
            }
            dump_receive_data(">");
        }

        public void characters(char[] ch, int start, int length) {
            stack.peek().add_text(ch, start, length);
            dump_receive_data(new String(ch, start, length));
        }

        public void endElement(String uri, String localName, String gName) {
            /* Pop element off stack */
            XMLElement element = stack.pop();
            String tag = element.get_tag();

            try {
                /* FIXME: Could we do this with a table lookup? */
                if ("SERVER".equals(tag))
                    handle_server(element);
                else if ("OPTIONS".equals(tag))
                    handle_options(element);
                else if ("MOTD".equals(tag))
                    handle_motd(element);
                else if ("RESULT".equals(tag))
                    handle_result(element);
                else if ("LIST".equals(tag))
                    handle_list(element);
                else if ("UPDATE".equals(tag))
                    handle_update(element);
                else if ("GAME".equals(tag))
                    handle_game(element);
                else if ("PROTOCOL".equals(tag))
                    handle_protocol(element);
                else if ("ALLOW".equals(tag))
                    handle_allow(element);
                else if ("ABOUT".equals(tag))
                    handle_about(element);
                else if ("BOT".equals(tag))
                    handle_bot(element);
                else if ("ROOM".equals(tag))
                    handle_room(element);
                else if ("PLAYER".equals(tag))
                    handle_player(element);
                else if ("TABLE".equals(tag))
                    handle_table(element);
                else if ("SEAT".equals(tag))
                    handle_seat(element);
                else if ("SPECTATOR".equals(tag))
                    handle_spectator_seat(element);
                else if ("LEAVE".equals(tag))
                    handle_leave(element);
                else if ("JOIN".equals(tag))
                    handle_join(element);
                else if ("CHAT".equals(tag))
                    handle_chat(element);
                else if ("INFO".equals(tag))
                    handle_info(element);
                else if ("PLAYERINFO".equals(tag))
                    handle_playerinfo(element);
                else if ("DESC".equals(tag))
                    handle_desc(element);
                else if ("PASSWORD".equals(tag))
                    handle_password(element);
                else if ("PING".equals(tag))
                    handle_ping(element);
                else if ("SESSION".equals(tag))
                    handle_session(element);
                // else
                // Ignore
            } catch (IOException e) {
                log.warning(e.toString());
            }

            // if (element.get_text() == null) {
            // dump_data(" />");
            // } else {
            dump_receive_data("</" + localName + ">");
            // }
        }

        public void error(SAXException e) {
            e.printStackTrace();
        }
    }

    Net(Server server, String host, int port, boolean use_tls) {
        /* Set fd to invalid value */
        this.fd = null;
        this.send_dump_file = null;
        this.receive_dump_file = null;
        this.server = server;
        this.host = host;
        this.port = port;
        this.use_tls = use_tls;
    }

    /**
     * Sets the files to output to. 'stderr' output to System.err 'stdout'
     * output to System.out
     * 
     * @param filename
     */
    void set_dump_files(String sendFilename, String receiveFilename)
            throws IOException {
        if (sendFilename == null) {
            this.send_dump_file = null;
        } else if ("stdout".equals(sendFilename)) {
            this.send_dump_file = new OutputStreamWriter(System.out);
        } else if ("stderr".equals(sendFilename)) {
            this.send_dump_file = new OutputStreamWriter(System.err);
        } else {
            try {
                this.send_dump_file = new OutputStreamWriter(
                        new FileOutputStream(sendFilename), "UTF-8");
            } catch (UnsupportedEncodingException e) {
                this.send_dump_file = new OutputStreamWriter(
                        new FileOutputStream(sendFilename));
            }
        }

        if (receiveFilename == null) {
            this.receive_dump_file = null;
        } else if (receiveFilename.equals(sendFilename)) {
            // Output both to same file.
            this.receive_dump_file = this.send_dump_file;
        } else if ("stdout".equals(receiveFilename)) {
            this.receive_dump_file = new OutputStreamWriter(System.out);
        } else if ("stderr".equals(receiveFilename)) {
            this.receive_dump_file = new OutputStreamWriter(System.err);
        } else {
            try {
                this.receive_dump_file = new OutputStreamWriter(
                        new FileOutputStream(receiveFilename), "UTF-8");
            } catch (UnsupportedEncodingException e) {
                this.receive_dump_file = new OutputStreamWriter(
                        new FileOutputStream(receiveFilename));
            }
        }
    }

    String get_host() {
        return this.host;
    }

    int get_port() {
        return this.port;
    }

    Socket get_fd() {
        return this.fd;
    }

    boolean get_tls() {
        return this.use_tls;
    }

    /* For debugging purposes only! */
    void set_fd(Socket fd) throws IOException {
        this.fd = fd;
        try {
            this.out = new OutputStreamWriter(fd.getOutputStream(), "UTF-8");
        } catch (UnsupportedEncodingException e) {
            log.warning(e.toString());
            this.out = new OutputStreamWriter(fd.getOutputStream());
        }
    }

    /* FIXME: set a timeout for connecting */
    void connect() throws IOException {
        log.fine("Connecting to " + this.host + ":" + this.port);
        this.fd = new Socket(this.host, this.port);
        this.out = new OutputStreamWriter(fd.getOutputStream(), "UTF-8");
        this.is_session_over = false;
        new Thread(this).start();
    }

    void disconnect() {
        log.fine("Disconnecting");
        try {
            fd.close();
        } catch (IOException e) {
            // ignore
        }
    }

    /* ggzcore_net_send_XXX() functions for sending messages to the server */

    /*
     * Sends login packet. Login type is an enumerated value. Password is needed
     * only for registered logins.
     */
    void send_login(LoginType login_type, String handle, String password,
            String email, String language) throws IOException {
        String type = "guest";

        switch (login_type) {
        case GGZ_LOGIN:
            type = "normal";
            break;
        case GGZ_LOGIN_NEW:
            type = "first";
            break;
        case GGZ_LOGIN_GUEST:
            type = "guest";
            break;
        }

        send_line("<LANGUAGE>" + language + "</LANGUAGE>");
        send_line("<LOGIN TYPE='" + type + "'>");
        send_line("<NAME>" + handle + "</NAME>");

        if ((login_type == LoginType.GGZ_LOGIN || (login_type == LoginType.GGZ_LOGIN_NEW))
                && password != null)
            send_line("<PASSWORD>" + password + "</PASSWORD>");
        if (login_type == LoginType.GGZ_LOGIN_NEW && email != null)
            send_line("<EMAIL>" + email + "</EMAIL>");

        send_line("</LOGIN>");

    }

    void send_channel(String id) throws IOException {
        send_line("<CHANNEL ID='" + id + "' />");
    }

    void send_motd() throws IOException {
        log.fine("Sending MOTD request");
        send_line("<MOTD/>");
    }

    void send_list_types(boolean verbose) throws IOException {
        log.fine("Sending gametype list request");

        send_line("<LIST TYPE='game' FULL='" + String.valueOf(verbose) + "'/>");
    }

    void send_list_rooms(boolean verbose) throws IOException {
        log.fine("Sending room list request");

        send_line("<LIST TYPE='room' FULL='" + String.valueOf(verbose) + "'/>");
    }

    void send_join_room(int room_id) throws IOException {
        log.fine("Sending room " + room_id + " join request");
        send_line("<ENTER ROOM='" + room_id + "'/>");
    }

    void send_list_players() throws IOException {
        log.fine("Sending player list request");
        send_line("<LIST TYPE='player'/>");
    }

    void send_list_tables() throws IOException {
        log.fine("Sending table list request");
        send_line("<LIST TYPE='table'/>");
    }

    /* Send a <CHAT> tag. */
    void send_chat(ChatType type, String player, String msg) throws IOException {
        String type_str;
        String chat_text;

        log.fine("Sending chat");

        type_str = type.toString();

        /*
         * Truncate a chat that is too long. It is left up to the caller to
         * split the text up into multiple packets (that's outside of our scope
         * here).
         */
        if (msg != null && msg.length() > this.chat_size) {
            /*
             * It would be much better to allocate this data on the stack, or to
             * modify msg directly, but neither is easily feasible - msg is
             * read-only, and allocating on the stack means lots of extra code
             * (since chat_size may be arbitrarily large).
             */
            log.warning("Truncating too-long chat message.");
            chat_text = msg.substring(0, chat_size);
        } else {
            chat_text = msg;
        }

        switch (type) {
        case GGZ_CHAT_NORMAL:
        case GGZ_CHAT_ANNOUNCE:
        case GGZ_CHAT_TABLE:
            send_line("<CHAT TYPE='" + type_str + "'>" + xml_escape(chat_text)
                    + "</CHAT>");
            break;
        case GGZ_CHAT_BEEP:
            send_line("<CHAT TYPE='" + type_str + "' TO='" + player + "'/>");
            break;
        case GGZ_CHAT_PERSONAL:
            send_line("<CHAT TYPE='" + type_str + "' TO='" + player + "'>"
                    + xml_escape(chat_text) + "</CHAT>");
            break;
        case GGZ_CHAT_UNKNOWN:
        default:
            /*
             * Returning an error would mean a *network* error, which isn't the
             * case.
             */
            log.warning("Unknown chat opcode " + type + " specified.");
            break;
        }

        // log.warning("ggzcore_net_send_chat: unknown chat type given.");
    }

    void send_player_info(int seat_num) throws IOException {

        log.fine("Sending player info request");

        if (seat_num == -1) {
            send_line("<INFO/>");
        } else {
            send_line("<INFO SEAT='" + seat_num + "'/>");
        }
    }

    void send_table_launch(Table table) throws IOException {
        int type, num_seats = 0;
        String desc;

        log.fine("Sending table launch request");

        type = table.get_type().get_id();
        desc = table.get_desc();
        num_seats = table.get_num_seats();

        send_line("<LAUNCH>");
        send_line("<TABLE GAME='" + type + "' SEATS='" + num_seats + "'>");
        if (desc != null)
            send_line("<DESC>" + desc + "</DESC>");

        for (int i = 0; i < num_seats; i++) {
            TableSeat seat = table.get_nth_seat(i);

            send_table_seat(seat);
        }

        send_line("</TABLE>");
        send_line("</LAUNCH>");
    }

    void send_table_seat(TableSeat seat) throws IOException {
        String type;

        log.fine("Sending seat info");

        type = seat.type.toString();

        if (seat.name == null)
            send_line("<SEAT NUM='" + seat.index + "' TYPE='" + type + "'/>");
        else
            send_line("<SEAT NUM='" + seat.index + "' TYPE='" + type + "'>"
                    + seat.name + "</SEAT>");
    }

    void send_table_join(int num, boolean spectator) throws IOException {
        log.fine("Sending table join request");
        send_line("<JOIN TABLE='" + num + "' SPECTATOR='"
                + String.valueOf(spectator) + "'/>");
    }

    void send_table_leave(boolean force, boolean spectator) throws IOException {
        log.fine("Sending table leave request");
        send_line("<LEAVE FORCE='" + String.valueOf(force) + "' SPECTATOR='"
                + String.valueOf(spectator) + "'/>");
    }

    void send_table_reseat(ReseatType opcode, int seat_num) throws IOException {
        String action = null;

        switch (opcode) {
        case GGZ_RESEAT_SIT:
            action = "sit";
            break;
        case GGZ_RESEAT_STAND:
            action = "stand";
            seat_num = -1;
            break;
        case GGZ_RESEAT_MOVE:
            action = "move";
            if (seat_num < 0)
                throw new IllegalArgumentException(
                        "seat_num cannot be less than 0");
            break;
        }

        if (action == null)
            throw new IllegalArgumentException("action cannot be null");

        if (seat_num < 0)
            send_line("<RESEAT ACTION='" + action + "'/>");

        send_line("<RESEAT ACTION='" + action + "' SEAT='" + seat_num + "'/>");
    }

    void send_table_seat_update(Table table, TableSeat seat) throws IOException {
        Room room = table.get_room();
        int room_id = room.get_id();
        int id = table.get_id();
        int num_seats = table.get_num_seats();

        log.fine("Sending table seat update request");
        send_line("<UPDATE TYPE='table' ACTION='seat' ROOM='" + room_id + "'>");
        send_line("<TABLE ID='" + id + "' SEATS='" + num_seats + "'>");
        send_table_seat(seat);
        send_line("</TABLE>");
        send_line("</UPDATE>");
    }

    void send_table_desc_update(Table table, String desc) throws IOException {
        Room room = table.get_room();
        int room_id = room.get_id();
        int id = table.get_id();

        log.fine("Sending table description update request");
        send_line("<UPDATE TYPE='table' ACTION='desc' ROOM='" + room_id + "'>");
        send_line("<TABLE ID='" + id + "'>");
        send_line("<DESC>" + desc + "</DESC>");
        send_line("</TABLE>");
        send_line("</UPDATE>");
    }

    void send_table_boot_update(Table table, TableSeat seat) throws IOException {
        Room room = table.get_room();
        int room_id = room.get_id();
        int id = table.get_id();

        log.fine("Sending boot of player " + seat.name);

        if (seat.name == null)
            throw new IllegalArgumentException("Seat cannot have a null name");
        seat.type = SeatType.GGZ_SEAT_PLAYER;
        seat.index = 0;

        send_line("<UPDATE TYPE='table' ACTION='boot' ROOM='" + room_id + "'>");

        send_line("<TABLE ID='" + id + "' SEATS='1'>");
        send_table_seat(seat);
        send_line("</TABLE>");

        send_line("</UPDATE>");
    }

    void send_logout() throws IOException {
        log.fine("Sending LOGOUT");
        send_line("</SESSION>");
    }

    protected void dump_receive_data(String data) {
        if (receive_dump_file != null) {
            try {
                receive_dump_file.write(data);
                receive_dump_file.flush();
            } catch (IOException e) {
                // Not a big deal if we can't write to the dump file but alert
                // the user anyway.
                log.warning(e.toString());
            }
        }
    }

    protected void dump_send_data(String data) {
        if (send_dump_file != null) {
            try {
                send_dump_file.write(data);
                send_dump_file.flush();
            } catch (IOException e) {
                // Not a big deal if we can't write to the dump file but alert
                // the user anyway.
                log.warning(e.toString());
            }
        }
    }

    /* Functions for <SERVER> tag */
    protected void handle_server(XMLElement element) throws IOException {
        String name, id, status, tls;
        int version;
        Integer chatlen;

        if (element == null)
            return;

        name = element.get_attr("NAME");
        id = element.get_attr("ID");
        status = element.get_attr("STATUS");
        version = str_to_int(element.get_attr("VERSION"), -1);
        tls = element.get_attr("TLS_SUPPORT");

        chatlen = (Integer) element.get_data();
        if (chatlen != null) {
            this.chat_size = chatlen.intValue();
        } else {
            /*
             * If no chat length is specified, assume an unlimited (i.e. really
             * large) length.
             */
            this.chat_size = Integer.MAX_VALUE;
        }

        log.fine(name + "(" + id + ") : status " + status + ": protocol "
                + version + ": chat size " + this.chat_size + " tls: " + tls);

        /* FIXME: Do something with name, status */
        if (version == GGZ_CS_PROTO_VERSION) {
            /* Everything checked out so start session */
            send_header();

            /* If TLS is enabled set it up */
            if ("yes".equals(tls) && get_tls()
            // && ggz_tls_support_query() TODO add TLS support one day.
            )
                negotiate_tls();

            this.server.set_negotiate_status(this, ClientReqError.E_OK);
        } else
            this.server.set_negotiate_status(this, ClientReqError.E_UNKNOWN);
    }

    /* Functions for <OPTIONS> tag */
    protected void handle_options(XMLElement element) {
        Integer chatlen;
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return; /* should this be an error? */
        parent_tag = parent.get_tag();
        if (!"SERVER".equals(parent_tag))
            return; /* should this be an error? */

        /* Read the server's maximum chat length. */
        chatlen = Integer.valueOf(element.get_attr("CHATLEN"));
        if (chatlen.intValue() < 0)
            return;

        parent.set_data(chatlen);
    }

    /* Functions for <MOTD> tag */
    protected void handle_motd(XMLElement element) {
        String message, priority, url;
        MotdEventData motd = new MotdEventData();

        message = element.get_text();
        priority = element.get_attr("PRIORITY");
        url = element.get_attr("URL");

        log.fine("Motd of priority " + priority);

        /*
         * In the old interface the MOTD was sent a line at a time,
         * null-terminated. Now it's just sent all at once.
         */
        if (url != null && url.length() == 0)
            url = null;
        motd.motd = message;
        motd.url = url;
        motd.priority = priority;

        /* FIXME: do something with the priority */
        this.server.event(ServerEvent.GGZ_MOTD_LOADED, motd);
    }

    /* Functions for <RESULT> tag */
    protected void handle_result(XMLElement element) {
        Room room;
        String action;
        ClientReqError code;

        if (element == null)
            return;

        action = element.get_attr("ACTION");
        code = ClientReqError.string_to_error(element.get_attr("CODE"));

        log.fine("Result of " + action + " was " + code);

        room = this.server.get_cur_room();

        if ("login".equals(action)) {
            /* Password may have already been updated. */
            this.server.set_login_status(code);
        } else if ("enter".equals(action)) {
            this.server.set_room_join_status(code);
        } else if ("launch".equals(action))
            room.set_table_launch_status(code);
        else if ("join".equals(action))
            room.set_table_join_status(code);
        else if ("leave".equals(action))
            room.set_table_leave_status(code);
        else if ("chat".equals(action)) {
            if (code != ClientReqError.E_OK) {
                ErrorEventData error = new ErrorEventData(code);

                switch (code) {
                case E_NOT_IN_ROOM:
                    error.message = "Not in a room";
                    break;
                case E_BAD_OPTIONS:
                    error.message = "Bad options";
                    break;
                case E_NO_PERMISSION:
                    error.message = "Prohibited";
                    break;
                case E_USR_LOOKUP:
                    error.message = "No such player";
                    break;
                case E_AT_TABLE:
                    error.message = "Can't chat at table";
                    break;
                case E_NO_TABLE:
                    error.message = "Must be at table";
                    break;
                default:
                    error.message = "Unknown error";
                    break;
                }
                this.server.event(ServerEvent.GGZ_CHAT_FAIL, error);
            }
        } else if ("protocol".equals(action)) {
            /* These are always errors */
            String message;
            switch (code) {
            case E_BAD_OPTIONS:
                message = "Server didn't recognize one of our commands";
                break;
            case E_BAD_XML:
                message = "Server didn't like our XML";
                break;
            default:
                message = "Unknown protocol error";
            }

            this.server.protocol_error(message);
        }
    }

    /* Functions for <PASSWORD> tag */
    protected void handle_password(XMLElement element) {
        String password;
        XMLElement parent;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        password = element.get_text();
        if (password == null)
            return;

        /*
         * This means the server can change our password at any time just by
         * sending us a <PASSWORD> tag. This could be bad, but fixing it so that
         * we only accept new passwords when we asked for them would be very
         * difficult. Plus, the password system needs to be changed anyway...
         */
        this.server.set_password(password);
    }

    /* Functions for <LIST> tag */
    protected void handle_list(XMLElement element) {
        List list;
        Iterator iter;
        Room room;
        int count, room_num;
        String type;

        if (element == null)
            return;

        /* Grab list data from tag */
        type = element.get_attr("TYPE");
        list = (List) element.get_data();
        room_num = str_to_int(element.get_attr("ROOM"), -1);

        /* Get length of list */
        if (list == null) {
            // HB Added to prevent NullPointerException
            list = new ArrayList(0);
        }
        count = list.size();

        if ("room".equals(type)) {
            /* Clear existing list (if any) */
            // if (this.server.get_num_rooms() > 0)
            // this.server.free_roomlist();
            this.server.init_roomlist(count);

            // for (entry = ggz_list_head(list); entry; entry =
            // ggz_list_next(entry)) {

            for (iter = list.iterator(); iter.hasNext();) {
                this.server.add_room((Room) iter.next());
            }
            this.server.event(ServerEvent.GGZ_ROOM_LIST, null);
        } else if ("game".equals(type)) {
            this.server.init_typelist(count);
            for (iter = list.iterator(); iter.hasNext();) {
                this.server.add_type((GameType) iter.next());
            }
            this.server.event(ServerEvent.GGZ_TYPE_LIST, null);
        } else if ("player".equals(type)) {
            room = this.server.get_room_by_id(room_num);
            room.set_player_list(count, list);
            list = null; /* avoid freeing list */
        } else if ("table".equals(type)) {
            room = this.server.get_room_by_id(room_num);
            room.set_table_list(list);
            list = null; /* avoid freeing list */
        }
    }

    private static void list_insert(XMLElement list_tag, Object data) {
        List list = (List) list_tag.get_data();

        /* If list doesn't already exist, create it */
        if (list == null) {
            /* Setup actual list */
            String type = list_tag.get_attr("TYPE");

            if ("game".equals(type)) {
                list = new ArrayList<GameType>();
            } else if ("room".equals(type)) {
                list = new ArrayList<Room>();
            } else if ("player".equals(type)) {
                list = new ArrayList<Player>();
                // C code sorted these by id
            } else if ("table".equals(type)) {
                list = new ArrayList<Table>();
                // C code sorted these by id
            }
            list_tag.set_data(list);
        }

        list.add(data);
    }

    /* Functions for <UPDATE> tag */
    protected void handle_update(XMLElement element) throws IOException {
        String action, type;

        /* Return if there's no tag */
        if (element == null)
            return;

        /* Grab update data from tag */
        type = element.get_attr("TYPE");
        action = element.get_attr("ACTION");

        if ("room".equals(type)) {
            room_update(element, action);
        } else if ("game".equals(type)) {
            /* FIXME: implement this */
        } else if ("player".equals(type))
            player_update(element, action);
        else if ("table".equals(type))
            table_update(element, action);
    }

    /* Handle room update. */
    void room_update(XMLElement update, String action) {
        Room roomdata, room;
        int id, players;

        roomdata = (Room) update.get_data();
        if (roomdata == null)
            return;
        id = roomdata.get_id();
        room = this.server.get_room_by_id(id);

        if (room != null) {
            if ("players".equals(action)) {
                players = roomdata.get_num_players();
                room.set_players(players);
            }
        }
    }

    /* Handle Player update */
    void player_update(XMLElement update, String action) {
        int room_num;
        Player player;
        Room room;
        String player_name;

        room_num = str_to_int(update.get_attr("ROOM"), -1);

        player = (Player) update.get_data();
        if (player == null) {
            return;
        }
        player_name = player.get_name();

        room = this.server.get_room_by_id(room_num);
        if (room == null) {
            return;
        }

        if ("add".equals(action)) {
            int from_room = str_to_int(update.get_attr("FROMROOM"), -2);

            room.add_player(player, from_room);
        } else if ("delete".equals(action)) {
            int to_room = str_to_int(update.get_attr("TOROOM"), -2);

            room.remove_player(player_name, to_room);
        } else if ("lag".equals(action)) {
            /* FIXME: Should be a player "class-based" event */
            int lag = player.get_lag();

            room.set_player_lag(player_name, lag);
        } else if ("stats".equals(action)) {
            /* FIXME: Should be a player "class-based" event */
            room.set_player_stats(player);
        }
    }

    /* Handle table update */
    private void table_update(XMLElement update, String action)
            throws IOException {
        int i, room_num, table_id;
        Table table, table_data;
        Room room;
        String room_str;

        /* Sanity check: we can't proceed without a room number */
        room_str = update.get_attr("ROOM");
        if (room_str == null) {
            /*
             * Assume we're talking about the current room. This is no doubt
             * preferable to simply dropping the connection.
             */
            room = this.server.get_cur_room();
            room_num = room.get_id();
        } else
            room_num = str_to_int(room_str, -1);

        room = this.server.get_room_by_id(room_num);
        if (room == null) {
            /*
             * We could use the current room in this case too, but that would be
             * more dangerous.
             */
            String msg = "Server specified non-existent room '" + room_str
                    + "'";
            this.server.protocol_error(msg);
            return;
        }

        table_data = (Table) update.get_data();
        table_id = table_data.get_id();
        table = room.get_table_by_id(table_id);

        /* Table can only be null if we're adding it */
        if (table == null && !"add".equals(action)) {
            String msg = "Server specified non-existent table " + table_id;
            this.server.protocol_error(msg);
            return;
        }

        if ("add".equals(action)) {
            room.add_table(table_data);
        } else if ("delete".equals(action)) {
            room.remove_table(table_id);
        } else if ("join".equals(action)) {
            /* Loop over both seats and spectators. */
            for (i = 0; i < table_data.get_num_seats(); i++) {
                TableSeat seat = table_data.get_nth_seat(i);

                if (seat.type != SeatType.GGZ_SEAT_NONE) {
                    table.set_seat(seat);
                }
            }
            for (i = 0; i < table_data.get_num_spectator_seats(); i++) {
                TableSeat spectator = table_data.get_nth_spectator_seat(i);

                if (spectator.name != null) {
                    table.set_spectator_seat(spectator);
                }
            }
        } else if ("leave".equals(action)) {
            /*
             * The server sends us the player that is leaving - that is, a
             * GGZ_SEAT_PLAYER not a GGZ_SEAT_OPEN. It may be either a regular
             * or a spectator seat.
             */
            for (i = 0; i < table_data.get_num_seats(); i++) {
                TableSeat leave_seat = table_data.get_nth_seat(i);

                if (leave_seat.type != SeatType.GGZ_SEAT_NONE) {
                    /* Player is vacating seat */
                    TableSeat seat = new TableSeat();
                    seat.index = i;
                    seat.type = SeatType.GGZ_SEAT_OPEN;
                    seat.name = null;
                    table.set_seat(seat);
                }
            }
            for (i = 0; i < table_data.get_num_spectator_seats(); i++) {
                TableSeat leave_spectator = table_data
                        .get_nth_spectator_seat(i);

                if (leave_spectator.name != null) {
                    /* Player is vacating seat */
                    TableSeat seat = new TableSeat();
                    seat.index = i;
                    seat.name = null;
                    table.set_spectator_seat(seat);
                }
            }
        } else if ("status".equalsIgnoreCase(action)) {
            table.set_state(table_data.get_state());
        } else if ("desc".equalsIgnoreCase(action)) {
            table.set_desc(table_data.get_desc());
        } else if ("seat".equalsIgnoreCase(action)) {
            for (i = 0; i < table_data.get_num_seats(); i++) {
                TableSeat seat = table_data.get_nth_seat(i);

                if (seat.type != SeatType.GGZ_SEAT_NONE) {
                    table.set_seat(seat);
                }
            }
        }
    }

    /* Functions for <GAME> tag */
    protected void handle_game(XMLElement element) {
        GameType type;
        GameData data;
        XMLElement parent;
        String parent_tag, parent_type;
        int id;
        String name, version;
        String prot_engine = null;
        String prot_version = null;
        NumberList player_allow_list = new NumberList();
        NumberList bot_allow_list = new NumberList();
        boolean spectators_allow = false;
        boolean peers_allow = false;
        String desc = null;
        String author = null;
        String url = null;
        int i;

        if (element == null)
            return;

        /* Get game data from tag */
        id = str_to_int(element.get_attr("ID"), -1);
        name = element.get_attr("NAME");
        version = element.get_attr("VERSION");
        data = (GameData) element.get_data();

        if (data != null) {
            prot_engine = data.prot_engine;
            prot_version = data.prot_version;
            player_allow_list = data.player_allow_list;
            bot_allow_list = data.bot_allow_list;
            spectators_allow = data.spectators_allow;
            peers_allow = data.peers_allow;
            desc = data.desc;
            author = data.author;
            url = data.url;
        }

        type = new GameType(id, name, version, prot_engine, prot_version,
                player_allow_list, bot_allow_list, spectators_allow,
                peers_allow, desc, author, url);

        if (data.named_bots != null) {
            for (i = 0; i < data.named_bots.length; i++) {
                type.add_namedbot(data.named_bots[i][0], data.named_bots[i][1]);
            }
        }

        /* Get parent off top of stack */
        parent = this.stack.peek();
        parent_tag = parent.get_tag();
        parent_type = parent.get_attr("TYPE");

        if (parent != null && "LIST".equalsIgnoreCase(parent_tag)
                && "game".equalsIgnoreCase(parent_type))
            list_insert(parent, type);
    }

    /*
     * This should not be called by the parent tag, but only by the child tags
     * to set data for the parent.
     */
    private static GameData game_get_data(XMLElement game) {
        GameData data = (GameData) game.get_data();

        /* If data doesn't already exist, create it */
        if (data == null) {
            data = new GameData();
            game.set_data(data);
        }

        return data;
    }

    private static PlayerInfoData playerinfo_get_data(XMLElement info) {
        PlayerInfoData data = (PlayerInfoData) info.get_data();

        /* If data doesn't already exist, create it */
        if (data == null) {
            data = new PlayerInfoData();
            info.set_data(data);

            data.infos = new ArrayList<PlayerInfo>();
        }

        return data;
    }

    private static void game_set_protocol(XMLElement game, String engine,
            String version) {
        GameData data = game_get_data(game);

        if (data.prot_engine == null)
            data.prot_engine = engine;
        if (data.prot_version == null)
            data.prot_version = version;
    }

    private static void game_set_allowed(XMLElement game, NumberList players,
            NumberList bots, boolean spectators, boolean peers) {
        GameData data = game_get_data(game);

        data.player_allow_list = players;
        data.bot_allow_list = bots;
        data.spectators_allow = spectators;
        data.peers_allow = peers;
    }

    private static void game_set_info(XMLElement game, String author, String url) {
        GameData data = game_get_data(game);

        if (data.author == null)
            data.author = author;
        if (data.url == null)
            data.url = url;
    }

    private static void game_add_bot(XMLElement game, String botname,
            String botclass) {
        GameData data = game_get_data(game);
        int size = 0;

        if (data.named_bots != null) {
            while (data.named_bots[size] != null)
                size++;
        }
        // data.named_bots = (char***)ggz_realloc(data.named_bots, (size + 2) *
        // sizeof(char**));
        String[][] old = data.named_bots;
        data.named_bots = new String[size + 2][2];
        System.arraycopy(old, 0, data.named_bots, 0, size);
        // data.named_bots[size] = (char**)ggz_malloc(2 * sizeof(char**));
        data.named_bots[size][0] = botname;
        data.named_bots[size][1] = botclass;
        data.named_bots[size + 1] = null;
    }

    private static void playerinfo_add_seat(XMLElement info, int num,
            String realname, String photo, String host) {
        PlayerInfoData data = playerinfo_get_data(info);
        PlayerInfo tmp = new PlayerInfo();

        tmp.num = num;
        tmp.realname = realname;
        tmp.photo = photo;
        tmp.host = host;

        data.infos.add(tmp);
    }

    private static void game_set_desc(XMLElement game, String desc) {
        GameData data = game_get_data(game);

        if (data.desc == null)
            data.desc = desc;
    }

    /* Functions for <PROTOCOL> tag */
    protected void handle_protocol(XMLElement element) {
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        parent_tag = parent.get_tag();
        if (!"GAME".equalsIgnoreCase(parent_tag))
            return;

        game_set_protocol(parent, element.get_attr("ENGINE"), element
                .get_attr("VERSION"));
    }

    /* Functions for <ALLOW> tag */
    protected void handle_allow(XMLElement element) {
        XMLElement parent;
        String parent_tag;
        NumberList players, bots;
        boolean spectators, peers;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        parent_tag = parent.get_tag();
        if (!"GAME".equalsIgnoreCase(parent_tag))
            return;

        players = NumberList.read(element.get_attr("PLAYERS"));
        bots = NumberList.read(element.get_attr("BOTS"));
        spectators = Boolean.valueOf(element.get_attr("SPECTATORS"));
        peers = Boolean.valueOf(element.get_attr("PEERS"));

        game_set_allowed(parent, players, bots, spectators, peers);
    }

    /* Functions for <ABOUT> tag */
    protected void handle_about(XMLElement element) {
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        parent_tag = parent.get_tag();
        if (!"GAME".equalsIgnoreCase(parent_tag))
            return;

        game_set_info(parent, element.get_attr("AUTHOR"), element
                .get_attr("URL"));
    }

    /* Functions for <BOT> tag */
    protected void handle_bot(XMLElement element) {
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        parent_tag = parent.get_tag();
        if (!"GAME".equalsIgnoreCase(parent_tag))
            return;

        game_add_bot(parent, element.get_attr("NAME"), element
                .get_attr("CLASS"));
    }

    /* Functions for <DESC> tag */
    protected void handle_desc(XMLElement element) {
        String desc;
        String parent_tag;
        XMLElement parent;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        desc = element.get_text();

        parent_tag = parent.get_tag();

        /* This tag can be a child of <GAME>, <ROOM>, or <TABLE>. */
        if ("GAME".equalsIgnoreCase(parent_tag))
            game_set_desc(parent, desc);
        else if ("ROOM".equalsIgnoreCase(parent_tag)) {
            if (parent.get_data() == null)
                parent.set_data(desc);
        } else if ("TABLE".equalsIgnoreCase(parent_tag))
            table_set_desc(parent, desc);
    }

    /* Functions for <ROOM> tag */
    protected void handle_room(XMLElement element) {
        Room ggz_room;
        int id, game, players;
        String name, desc;
        XMLElement parent = this.stack.peek();
        String parent_tag, parent_type;

        if (element == null)
            return;

        if (parent == null) {
            return;
        }

        /* Grab data from tag */
        id = str_to_int(element.get_attr("ID"), -1);
        name = element.get_attr("NAME");
        game = str_to_int(element.get_attr("GAME"), -1);
        desc = (String) element.get_data();
        players = str_to_int(element.get_attr("PLAYERS"), -1);

        /* Set up GGZRoom object */
        ggz_room = new Room(this.server, id, name, game, desc, players);

        /* Get parent off top of stack */
        parent_tag = parent.get_tag();
        parent_type = parent.get_attr("TYPE");

        if ("LIST".equalsIgnoreCase(parent_tag)
                && "room".equalsIgnoreCase(parent_type)) {
            list_insert(parent, ggz_room);
        } else if ("UPDATE".equalsIgnoreCase(parent_tag)
                && "room".equalsIgnoreCase(parent_type)
                && parent.get_data() == null) {
            parent.set_data(ggz_room);
        }
    }

    /* Functions for <PLAYER> tag */
    protected void handle_player(XMLElement element) {
        Player ggz_player;
        PlayerType type;
        Room room;
        String name, str_type;
        int table, lag;
        XMLElement parent;
        String parent_tag, parent_type;
        int wins, losses, ties, forfeits, rating, ranking, highscore;

        if (element == null)
            return;

        room = this.server.get_cur_room();

        /* Grab player data from tag */
        str_type = element.get_attr("TYPE");
        name = element.get_attr("ID");
        table = str_to_int(element.get_attr("TABLE"), -1);
        lag = str_to_int(element.get_attr("LAG"), 0);

        /* Set player's type */
        type = PlayerType.string_to_playertype(str_type);

        /* Set up GGZPlayer object */
        ggz_player = new Player(name, room, table, type, lag);

        /* FIXME: should these be initialized through an accessor function? */
        wins = str_to_int(element.get_attr("WINS"), Player.NO_RECORD);
        ties = str_to_int(element.get_attr("TIES"), Player.NO_RECORD);
        losses = str_to_int(element.get_attr("LOSSES"), Player.NO_RECORD);
        forfeits = str_to_int(element.get_attr("FORFEITS"), Player.NO_RECORD);
        rating = str_to_int(element.get_attr("RATING"), Player.NO_RATING);
        ranking = str_to_int(element.get_attr("RANKING"), Player.NO_RANKING);
        highscore = str_to_int(element.get_attr("HIGHSCORE"),
                Player.NO_HIGHSCORE);
        ggz_player.init_stats(wins, losses, ties, forfeits, rating, ranking,
                highscore);

        /* Get parent off top of stack */
        parent = this.stack.peek();
        parent_tag = parent.get_tag();
        parent_type = parent.get_attr("TYPE");

        if (parent != null && "LIST".equalsIgnoreCase(parent_tag)
                && "player".equalsIgnoreCase(parent_type))
            list_insert(parent, ggz_player);
        else if (parent != null && "UPDATE".equalsIgnoreCase(parent_tag)
                && "player".equalsIgnoreCase(parent_type))
            parent.set_data(ggz_player);
    }

    /* Functions for <TABLE> tag */
    protected void handle_table(XMLElement element) {
        GameType type;
        TableData data;
        Table table_obj;
        List<TableSeat> seats = null;
        List<TableSeat> spectatorseats = null;
        Iterator<TableSeat> iter;
        int id, game, num_seats;
        TableState status;
        String desc = null;
        XMLElement parent;
        String parent_tag, parent_type;

        if (element == null)
            return;

        /* Grab table data from tag */
        id = str_to_int(element.get_attr("ID"), -1);
        game = str_to_int(element.get_attr("GAME"), -1);
        status = TableState.valueOf(str_to_int(element.get_attr("STATUS"), 0));
        num_seats = str_to_int(element.get_attr("SEATS"), 0);
        data = (TableData) element.get_data();
        if (data != null) {
            desc = data.desc;
            seats = data.seats;
            spectatorseats = data.spectatorseats;
        }

        /* Create new table structure */
        // table_obj = new Table();
        type = this.server.get_type_by_id(game);
        // table_obj.init(type, desc, num_seats, status, id);
        table_obj = new Table(type, desc, num_seats, status, id);

        if (seats != null) {
            /* Add seats */
            iter = seats.iterator();
            while (iter.hasNext()) {
                TableSeat seat = iter.next();
                table_obj.set_seat(seat);
            }
        }

        if (spectatorseats != null) {
            /* Add spectator seats */
            iter = spectatorseats.iterator();
            while (iter.hasNext()) {
                TableSeat seat = iter.next();
                table_obj.set_spectator_seat(seat);
            }
        }

        /* Get parent off top of stack */
        parent = this.stack.peek();
        parent_tag = parent.get_tag();
        parent_type = parent.get_attr("TYPE");

        if (parent != null && "LIST".equalsIgnoreCase(parent_tag)
                && "table".equalsIgnoreCase(parent_type)) {
            list_insert(parent, table_obj);
        } else if (parent != null && "UPDATE".equalsIgnoreCase(parent_tag)
                && "table".equalsIgnoreCase(parent_type)) {
            parent.set_data(table_obj);
        }
    }

    private static TableData table_get_data(XMLElement table) {
        TableData data = (TableData) table.get_data();

        /* If data doesn't already exist, create it */
        if (data == null) {
            data = tabledata_new();
            table.set_data(data);
        }

        return data;
    }

    private static void table_add_seat(XMLElement table, TableSeat seat,
            boolean spectator) {
        TableData data = table_get_data(table);

        if (spectator) {
            data.spectatorseats.add(seat);
        } else {
            data.seats.add(seat);
        }
    }

    private static void table_set_desc(XMLElement table, String desc) {
        TableData data = table_get_data(table);

        if (data.desc == null)
            data.desc = desc;
    }

    private static TableData tabledata_new() {
        TableData data = new TableData();

        data.seats = new ArrayList<TableSeat>();
        data.spectatorseats = new ArrayList<TableSeat>();

        return data;
    }

    /* Functions for <SEAT> tag */
    protected void handle_seat(XMLElement element) {
        TableSeat seat_obj = new TableSeat();
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        /* Make sure parent is <TABLE> */
        parent_tag = parent.get_tag();
        if (parent_tag == null || !"TABLE".equalsIgnoreCase(parent_tag))
            return;

        /* Get seat information out of tag */
        seat_obj.index = str_to_int(element.get_attr("NUM"), -1);
        seat_obj.type = SeatType.string_to_seattype(element.get_attr("TYPE"));
        seat_obj.name = element.get_text();
        table_add_seat(parent, seat_obj, false);
    }

    /* Functions for <SPECTATOR> tag */
    protected void handle_spectator_seat(XMLElement element) {
        TableSeat seat_obj = new TableSeat();
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        /* Make sure parent is <TABLE> */
        parent_tag = parent.get_tag();
        if (parent_tag == null || !"TABLE".equalsIgnoreCase(parent_tag))
            return;

        /* Get seat information out of tag */
        seat_obj.index = str_to_int(element.get_attr("NUM"), -1);
        seat_obj.name = element.get_text();
        table_add_seat(parent, seat_obj, true);
    }

    protected void handle_leave(XMLElement element) {
        Room room;
        LeaveType reason;
        String player;

        if (element == null)
            return;

        room = this.server.get_cur_room();
        reason = LeaveType.string_to_leavetype(element.get_attr("REASON"));
        player = element.get_attr("PLAYER");

        room.set_table_leave(reason, player);
    }

    protected void handle_join(XMLElement element) throws IOException {
        Room room;
        int table;

        if (element == null)
            return;

        room = this.server.get_cur_room();
        table = str_to_int(element.get_attr("TABLE"), -1);

        room.set_table_join(table);
    }

    /* Functions for <CHAT> tag */
    protected void handle_chat(XMLElement element) {
        String msg, type_str, from;
        Room room;
        ChatType type;

        if (element == null)
            return;

        /* Grab chat data from tag */
        type_str = element.get_attr("TYPE");
        from = element.get_attr("FROM");
        msg = element.get_text();

        log.fine(type_str + " message from " + from + ": '" + msg + "'");

        type = ChatType.string_to_chattype(type_str);

        if (from == null && type != ChatType.GGZ_CHAT_UNKNOWN) {
            /* Ignore any message that has no sender. */
            return;
        }

        if (msg == null && type != ChatType.GGZ_CHAT_BEEP
                && type != ChatType.GGZ_CHAT_UNKNOWN) {
            /*
             * Ignore an empty message, except for the appropriate chat types.
             */
            return;
        }

        room = this.server.get_cur_room();
        room.add_chat(type, from, msg);
    }

    /* Functions for <INFO> tag */
    protected void handle_info(XMLElement element) {
        PlayerInfoData data = playerinfo_get_data(element);

        Game game = this.server.get_cur_game();
        game.set_info(data.infos.size(), data.infos);
    }

    /* Functions for <PLAYERINFO> tag */
    protected void handle_playerinfo(XMLElement element) {
        XMLElement parent;
        String parent_tag;

        if (element == null)
            return;

        /* Get parent off top of stack */
        parent = this.stack.peek();
        if (parent == null)
            return;

        parent_tag = parent.get_tag();
        if (!"INFO".equalsIgnoreCase(parent_tag))
            return;

        playerinfo_add_seat(parent, str_to_int(element.get_attr("SEAT"), -1),
                element.get_attr("REALNAME"), element.get_attr("PHOTO"),
                element.get_attr("HOST"));
    }

    /* Function for <PING> tag */
    protected void handle_ping(XMLElement element) throws IOException {
        /* No need to bother the client or anything, just send pong */
        String id = element.get_attr("ID");
        send_pong(id);
    }

    /* Function for <SESSION> tag */
    protected void handle_session(XMLElement element) {
        /* Note we only get this after </SESSION> is sent. */
        this.server.session_over(this);
        this.is_session_over = true;
    }

    /* Send the session header */
    private void send_header() throws IOException {
        send_line("<?xml version='1.0' encoding='UTF-8'?>");
        send_line("<SESSION>");
    }

    private void send_pong(String id) throws IOException {
        if (id != null)
            send_line("<PONG ID='" + id + "'/>");
        else
            send_line("<PONG/>");
    }

    /* Send a TLS_START notice and negotiate the handshake */
    void negotiate_tls() {
        throw new UnsupportedOperationException(
                "TNS not supported yet since I don't understand it.");
        // int ret;
        //
        // send_line("<TLS_START/>");
        // /* This should return a status one day to tell client if */
        // /* the handshake failed for some reason */
        // ret =
        // ggz_tls_enable_fd(this.fd, GGZ_TLS_CLIENT,
        // GGZ_TLS_VERIFY_NONE);
        // if (!ret)
        // this.use_tls = false;
    }

    private void send_line(String line) throws IOException {
        out.write(line);
        out.write("\n");
        out.flush();
        dump_send_data(line);
        dump_send_data("\n");
    }

    static int str_to_int(String str, int dflt) {
        int val;

        if (str == null)
            return dflt;
        try {
            val = Integer.parseInt(str);
        } catch (NumberFormatException e) {
            return dflt;
        }
        return val;
    }

    /**
     * If the the text contains the ]]> character sequence then the text is
     * split into multiple CDATA sections, effectively escaping the sequence.
     * e.g. some ]]> text is marshalled as <richText><![CDATA[some ]]]]><![CDATA[>
     * text]]></richText>
     * 
     * @return
     * @author Helg Bredow
     */
    static String xml_escape(String text) {
        // return text
        // .replaceAll("&", "&amp;")
        // .replaceAll("<", "&lt;")
        // .replaceAll(">", "&gt;")
        // .replaceAll("\"", "&quot;");
        //
        int splitPos = text.indexOf("]]>");

        if (splitPos > -1) {
            // Only create buffer if we need it to reduce garbage.
            StringBuffer buffer = new StringBuffer();
            while (splitPos > -1) {
                buffer.append("<![CDATA[");
                buffer.append(text.substring(0, splitPos + 2));
                buffer.append("]]>");
                text = text.substring(splitPos + 2);
                splitPos = text.indexOf("]]>");
            }
            buffer.append("<![CDATA[");
            buffer.append(text);
            buffer.append("]]>");
            return buffer.toString();
        }
        return "<![CDATA[" + text + "]]>";
    }

    /**
     * Main message pump.
     */
    public void run() {
        /* Initialize stack for messages */
        this.stack = new Stack<XMLElement>();
        try {
            try {
                XMLReader saxParser = XMLReaderFactory.createXMLReader();
                XML_Parser parser = new XML_Parser();
                InputSource input = new InputSource(new InputStreamReader(
                        new FilterInputStream(fd.getInputStream()) {
                            public int read(byte[] b, int off, int len)
                                    throws IOException {
                                // We need to return -1 to indicate EOF so the
                                // parser stops parsing, but the real EOF on the
                                // socket has not been reached.
                                if (Net.this.is_session_over) {
                                    return -1;
                                }
                                return super.read(b, off, len);
                            }

                            public void close() throws IOException {
                                if (Net.this == server.channel
                                        && Net.this.is_session_over) {
                                    // Do nothing.
                                } else {
                                    super.close();
                                }
                            }
                        }, "UTF-8"));
                saxParser.setContentHandler(parser);
                saxParser.setErrorHandler(parser);
                saxParser.parse(input);
                // } catch (SocketException e) {
                // // Assume socket closed by remote host.
                // e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
                this.server.protocol_error("Server disconnected");
                this.disconnect();
                this.server.session_over(this);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
