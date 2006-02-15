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

import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class Mod implements ModGGZ, ModGame {
	/** The state of the game. */
	private ModState state = ModState.GGZMOD_STATE_CREATED;

	private String my_name;

	private boolean i_am_spectator;

	private int my_seat_num;

	private Socket server_fd;

	// private String server_host;
	//
	// private int server_port;
	//
	// private String server_handle;

	private String class_name;

	private ModTransactionHandler thandler;

	private ModEventHandler ehandler;

	/* Seat and spectator seat data. */
	private int num_seats;

	private ArrayList seats;

	// private List stats;
	// private List infos;
	private int num_spectator_seats;

	private ArrayList spectator_seats;

	public ModState get_state() {
		return this.state;
	}

	public void add_mod_listener(ModTransactionHandler handler) {
		if (thandler != null) {
			throw new UnsupportedOperationException(
					"multiple handlers not supported yet");
		}
		this.thandler = handler;
	}

	public void add_mod_event_handler(ModEventHandler handler) {
		if (ehandler != null) {
			throw new UnsupportedOperationException(
					"multiple mod event handlers not supported yet");
		}
		this.ehandler = handler;
	}

	public void set_player(String name, boolean is_spectator, int seat_num) {
		this.my_name = name;
		this.i_am_spectator = is_spectator;
		this.my_seat_num = seat_num;

		if (this.state != ModState.GGZMOD_STATE_CREATED) {
			// _io_ggz_send_player(this.fd, name, is_spectator, seat_num);
			if (ehandler != null) {
				ehandler.handle_player(name, is_spectator, seat_num);
			}
		}
	}

	/**
	 * @brief Get data about this player.
	 * 
	 * Call this function to find out where at the table this player is sitting.
	 * @param ggzmod
	 *            The GGZMod object.
	 * @param is_spectator
	 *            Will be set to TRUE iff player is spectating.
	 * @param seat_num
	 *            Will be set to the number of our (spectator) seat.
	 * @return The name of the player (or NULL on error).
	 */
	public String get_player() {
		if (this.state == ModState.GGZMOD_STATE_CREATED) {
			throw new IllegalStateException(
					"ggzmod_get_my_seat: can't call when state is CREATED.");
		}
		return this.my_name;
	}

	public boolean is_spectator() {
		if (this.state == ModState.GGZMOD_STATE_CREATED) {
			throw new IllegalStateException(
					"ggzmod_get_my_seat: can't call when state is CREATED.");
		}
		return this.i_am_spectator;
	}

	public int get_seat_num() {
		if (this.state == ModState.GGZMOD_STATE_CREATED) {
			throw new IllegalStateException(
					"ggzmod_get_my_seat: can't call when state is CREATED.");
		}
		return this.my_seat_num;
	}

	public void connect() throws InstantiationException,
			IllegalAccessException, InvocationTargetException,
			NoSuchMethodException, ClassNotFoundException, IOException {
		/* For the ggz side, we fork the game and then send the launch message */

		Class c = Class.forName(this.class_name);
		Constructor constructor = c
				.getConstructor(new Class[] { ModGame.class });
		constructor.newInstance(new Object[] { this });
		send_game_launch();
		// if (ggzmod->argv) {
		// if (game_fork(ggzmod) < 0) {
		// _ggzmod_ggz_error(ggzmod, "Error: table fork failed");
		// return -1;
		// }
		// } else {
		// ggz_debug("GGZMOD", "Running embedded game (no fork)");
		// if (game_embedded(ggzmod) < 0) {
		// _ggzmod_ggz_error(ggzmod, "Error: embedded table failed");
		// return -1;
		// }
		// }

		// if (send_game_launch(ggzmod) < 0) {
		// _ggzmod_ggz_error(ggzmod, "Error sending launch to game");
		// return -1;
		// }
		//        
		// return 0;
	}

	public void disconnect() {
		// // Let the game know we're disconnecting.
		// ehandler.handle_disconnect();
	}

	public void inform_chat(String player, String msg) {
		// if (_io_ggz_send_msg_chat(ggzmod->fd, player, msg) < 0) {
		// return -1;
		// }
		// return 0;
		ehandler.handle_chat(player, msg);
	}

	public void set_seat(Seat seat) {
		Seat oldseat;

		if (seat == null || seat.num < 0) {
			throw new IllegalArgumentException("Seat is null or has num < 0");
		}

		oldseat = get_seat(seat.num);

		if (oldseat != null && oldseat.equals(seat)) {
			return;
		}

		if (seat.num >= this.num_seats) {
			this.num_seats = seat.num + 1;

			if (this.seats == null) {
				this.seats = new ArrayList();
			}

			this.seats.add(seat);
		} else {
			this.seats.set(seat.num, seat);
		}

		if (this.state != ModState.GGZMOD_STATE_CREATED) {
			ehandler.handle_seat(seat);
			// if (_io_ggz_send_seat(this.fd, seat) < 0)
			// _error("Error writing to game");
		}
	}

	public void set_spectator_seat(SpectatorSeat seat) {
		if (seat == null)
			throw new IllegalArgumentException("seat cannot be null");
		if (seat.get_num() < 0)
			throw new IllegalArgumentException("seat.num cannot be less than 0");

		if (this.state != ModState.GGZMOD_STATE_CREATED) {
			SpectatorSeat old_seat;
			old_seat = get_spectator_seat(seat.get_num());
			if (old_seat == null
					|| !seat.get_name().equals(old_seat.get_name())) {
				ehandler.handle_spectator_seat(seat);
				// _io_ggz_send_spectator_seat(ggzmod->fd, seat) < 0) {
				// _ggzmod_ggz_error(ggzmod, "Error writing to game");
				// return -4;
			}
		}

		if (seat.get_name() != null) {
			if (seat.get_num() >= this.num_spectator_seats) {
				this.num_spectator_seats = seat.get_num() + 1;
			}
			if (this.spectator_seats == null) {
				this.spectator_seats = new ArrayList();
			}
			this.spectator_seats.add(seat);
		} else if (this.spectator_seats != null) {
			/* Non-occupied seats are just empty entries in the list. */
			int entry = this.spectator_seats.indexOf(seat);
			if (entry > -1) {
				this.spectator_seats.remove(entry);
				this.num_spectator_seats--;
			}
		}
	}

	/**
	 * Get all data for the specified seat.
	 * 
	 * @param num
	 *            The seat number (0..(number of seats - 1)).
	 * @return A valid GGZSeat structure, if seat is a valid seat.
	 */
	public Seat get_seat(int num) {
		if (num >= 0 && num < this.num_seats) {
			for (int seat_num = 0; seat_num < this.seats.size(); seat_num++) {
				Seat seat = (Seat) this.seats.get(seat_num);
				if (num == seat.num) {
					return seat;
				}
			}
		}
		return null;
	}

	/**
	 * @brief Get the total number of seats at the table.
	 * @return The number of seats, or -1 on error.
	 * @note If no connection is present, -1 will be returned.
	 * @note While in GGZMOD_STATE_CREATED, we don't know the number of seats.
	 */
	public int get_num_seats() {
		/*
		 * Note: num_seats is initialized to 0 and isn't changed until we hear
		 * differently from GGZ.
		 */
		return this.num_seats;
	}

	/**
	 * @brief Get the maximum number of spectators. This function returns the
	 *        maximum number of spectator seats available. A game can use this
	 *        to iterate over the spectator seats to look for spectators
	 *        occupying them. Since spectators may come and go at any point and
	 *        there is no limit on the number of spectators, you should consider
	 *        this value to be dynamic and call this function again each time
	 *        you're looking for spectators.
	 * @return The number of available spectator seats, or -1 on error.
	 */
	public int get_num_spectator_seats() {
		/*
		 * Note: num_spectator_seats is initialized to 0 and isn't changed until
		 * we hear differently from GGZ.
		 */
		return this.num_spectator_seats;
	}

	/**
	 * @brief Get a spectator's data.
	 * @param ggzmod
	 *            The GGZMod object.
	 * @param seat
	 *            The number, between 0 and (number of spectators - 1).
	 * @return A valid GGZSpectator structure, if given a valid seat.
	 */
	public SpectatorSeat get_spectator_seat(int num) {
		SpectatorSeat seat = new SpectatorSeat(num, null);

		if (num >= 0 && num < this.num_spectator_seats) {
			int entry;
			entry = this.spectator_seats.indexOf(seat);
			if (entry > -1)
				seat = (SpectatorSeat) this.spectator_seats.get(entry);
		}

		return null;
	}

	public void set_state(ModState state) throws IOException {
		ModState old_state = this.state;
		if (state == this.state)
			return; /* Is this an error? */

		/*
		 * The callback function retrieves the state from get_state. It could
		 * instead be passed as an argument.
		 */
		this.state = state;
		thandler.handle_state(old_state);
	}

	public void set_module(String pwd, String class_name) {
		/* Check parameters */
		if (class_name == null) {
			throw new IllegalArgumentException("class_name cannot be null");
		}

		this.class_name = class_name;
		// ggzmod->pwd = ggz_strdup(pwd);
	}

	public void set_info(int num, List infos) {

		// _io_ggz_send_msg_info(ggzmod->fd, num, infos);
		ehandler.handle_info(num, infos);
	}

	public void set_server_fd(Socket fd) throws IOException {
		this.server_fd = fd;
		/* If we're already connected, send the fd */
		if (this.state == ModState.GGZMOD_STATE_CONNECTED)
			ehandler.handle_server_fd(fd);
	}

	// public void send_state(ModState new_state) throws IOException {
	// /*
	// * There's only certain ones the game is allowed to set it to, and they
	// * can only change it if the state is currently WAITING or PLAYING.
	// */
	// switch (new_state) {
	// case GGZMOD_STATE_CREATED:
	// case GGZMOD_STATE_CONNECTED:
	// case GGZMOD_STATE_WAITING:
	// case GGZMOD_STATE_PLAYING:
	// case GGZMOD_STATE_DONE:
	// /*
	// * In contradiction to what I say above, the game actually _is_
	// * allowed to change its state from CREATED to WAITING. When
	// * ggzmod-ggz sends a launch packet to ggzmod-game, ggzmod-game
	// * automatically changes the state from CREATED to WAITING. When
	// * this happens, it tells ggzmod-ggz of this change and we end up
	// * back here. So, although it's a bit unsafe, we have to allow this
	// * for now. The alternative would be to have ggzmod-ggz and
	// * ggzmod-game both separately change states when the launch packet
	// * is sent.
	// */
	// set_state(new_state);
	// return;
	// }
	// log.severe("Game requested incorrect state value");
	//
	// /* Is this right? has the gameover happened yet? */
	// }

	/*
	 * Sends a game launch packet to ggzmod-game.
	 */
	private void send_game_launch() throws IOException {

		ehandler.handle_player(this.my_name, this.i_am_spectator,
				this.my_seat_num);

		if (this.seats != null) {
			for (Iterator iter = this.seats.iterator(); iter.hasNext();) {
				Seat seat = (Seat) iter.next();
				ehandler.handle_seat(seat);
			}
		}

		if (this.spectator_seats != null) {
			for (Iterator iter = this.spectator_seats.iterator(); iter
					.hasNext();) {
				SpectatorSeat seat = (SpectatorSeat) iter.next();
				ehandler.handle_spectator_seat(seat);
			}
		}

		ehandler.handle_launch();

		/* If the server fd has already been set, send that too */
		if (this.server_fd != null) {
			ehandler.handle_server_fd(this.server_fd);
		}

		// if (this.server_host != null) {
		// ehandler.handle_server(this.server_host, this.server_port,
		// this.server_handle);
		// }
	}

	public void request_stand() throws IOException {
		thandler.handle_stand();
	}

	public void request_sit(int seat_num) throws IOException {
		thandler.handle_sit(seat_num);
	}

	public void request_boot(String name) throws IOException {
		thandler.handle_boot(name);
	}

	public void request_bot(int seat_num) throws IOException {
		thandler.handle_seatchange(ModTransaction.GGZMOD_TRANSACTION_BOT,
				seat_num);
	}

	public void request_open(int seat_num) throws IOException {
		thandler.handle_seatchange(ModTransaction.GGZMOD_TRANSACTION_OPEN,
				seat_num);
	}

	public void request_chat(String chat_msg) throws IOException {
		thandler.handle_chat(chat_msg);
	}

	public void player_request_info(int seat_num) throws IOException {
		thandler.handle_info(seat_num);
	}

}