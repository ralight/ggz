/*
 * File: common.h
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Frontend to GGZCards Client-Common
 * $Id: common.h 2990 2001-12-23 04:21:52Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <ggzmod.h>
#include "protocol.h"

#ifndef __COMMON_H__
#define __COMMON_H__


/** @defgroup Player
 *  A "player" as seen by the client is really just a seat at the
 *  table, complete with a hand and everything.  It's not necessary
 *  that a player actually be sitting at that seat - there may just be
 *  a pile of cards.
 *  @note We are always player 0.  The server will deal with all conversions.
 *  @{ */

/** Hand structure.
 *  @todo Should this be merged into struct seat_t?
 */
typedef struct {
	int hand_size;		/**< the number of cards in the hand */
	card_t *card;		/**< the list of cards */
} hand_t;

/** Contains all information about a seat at the table. */
typedef struct seat_t {
	GGZSeatType status;	/**< ggz seating assignment info */
	char *name;		/**< player's name */
	card_t table_card;	/**< card on table */
	hand_t hand;		/**< player's hand */
} seat_t;

/** @} end of Player group */


/** @defgroup Game
 *  Here we see global game data.
 *  @{ */

/** GGZCards client game states
 *  @note Any additional state data should be stored separately, while maintaining the state here. */
typedef enum {
	STATE_INIT,		/**< game hasn't started yet */
	STATE_WAIT,		/**< waiting for others */
	STATE_PLAY,		/**< our turn to play */
	STATE_BID,		/**< our turn to bid */
	STATE_DONE,		/**< game's over */
	STATE_OPTIONS	/**< determining options */
} client_state_t;

/** The game_t structure contains all global game data. */
struct ggzcards_game_t {
	int num_players;	/**< The number of players in the game. */
	seat_t *players;	/**< Data about each player */
	client_state_t state;	/**< The state the game is in */
	int play_hand;		/**< The hand we're currently playing from */
};

/** This is the game structure that contains all the common information
 *  about the ongoing game */
extern struct ggzcards_game_t ggzcards;

/** @} end of Game group */


/** @defgroup Setup
 *  Setup functions to be called by the frontend
 *  @{ */

/** This function should be called when the client first launches.  It
  * initializes all internal data and makes the connection to the GGZ
  * client.
  * @return The file descriptor that is used to communicate with the server. */
int client_initialize(void);

/** This function should be called on any input from the server fd.  It
 *  will do all the rest of the work, and call one of the callbacks if
 *  necessary.
 *  @see Callbacks */
int client_handle_server(void);

/** This function should be called just before the client exits. */
void client_quit(void);

/** @} end of Setup */


/** @defgroup Callbacks
 * Frontend-specific functions called by the common code.  It is
 * up to the frontend to define these functions appropriately.
 * @note These can easily be changed so that they'll fit better with other frontends.
 * @{ */

/** Handles a newgame request by calling client_send_newgame when
 *  ready (you may wish to ask the user first). */
extern void game_get_newgame(void);

/** Alerts the table to the start of a new game. */
extern void game_alert_newgame(void);

/** Handles a gameover message.
  * @param num_winners The number of players who won the game (0 or more)
  * @param winners An array with the player numbers of the winners. */
extern void game_handle_gameover(int num_winners, int *winners);

/** Alerts the table of a player's name and status, before changing that
  * name in the game structure.
  * @param player The number of the player whose name we're talking about.
  * @param name The (possibly new) name of the player. */
extern void game_alert_player(int player, GGZSeatType status,
			      const char *name);

/** Alert the table that the number of players has changed.  The
 *  table will probably want to redesign itself.
 *  @param new The new number of players.
 *  @param old The previous number of players.
 *  @note old and new will never be equal.
 *  @note ggzcards.num_players will have been updated. */
extern void game_alert_num_players(int new, int old);

/** Alerts the table to the maximum hand size.  There will never be more
 *  than this many cards in a hand (unless we send another alert). */
extern void game_alert_hand_size(int max_hand_size);

/** Called when the hand is changed; the frontend should draw/update it.
 *  @param player The player number of the player whose hand has changed. */
extern void game_display_hand(int player);

/** Called to request a bid.  The frontend should call client_send_bid at
  * any point afterwards to send the response.
  * @param possible_bids The number of possible bid choices.
  * @param bid_choices An array of strings, one corresponding to each bid choice. */
extern void game_get_bid(int possible_bids, char **bid_choices);

/** Called to request a play.  The frontend should call client_send_play
 *  at any point afterwards to send the response.
 *  @param hand The player number of the hand to play from. */
extern void game_get_play(int hand);

/** Called when there's a bad play.  The frontend should display the
 *  error message, and call client_send_play again.  It may be necessary
 *  to redraw cards.
 *  @param err_msg The error message, as sent by the server. */
extern void game_alert_badplay(char *err_msg);

/** Called when we're informed of a play.  The hand itself has already
 *  been updated; the frontend should redraw it and perhaps draw animation.
 *  @param player The player whose hand was played from.
 *  @param card The card that was played.
 *  @param pos The (former) position of the card. */
extern void game_alert_play(int player, card_t card, int pos);

/** Called when we're informaed of the table cards.  The information
 *  itself resides in the player structures; all the frontend has to
 *  do is redraw the cards. */
extern void game_alert_table(void);

/** Called when we're informed of a trick being over.  All game
 *  information wil be updated; the frontend will have to clear
 *  the table cards from the table and alert the player to who
 *  won.
 *  @param player The player who won the trick. */
extern void game_alert_trick(int player);

/** Called to request options.  The frontend should ask the client
 *  what options are desired, and then call client_send_options.
 *  @param option_cnt The number of options.
 *  @param choice_cnt The number of choices for each option.
 *  @param defaults The default choice for each option.
 *  @param option_choices Text message for each choice of each option.
 *  @return 0 if you are going to handle the option request, -1 if you aren't
 *  @note All parameters are freed after the function returns. */
extern int game_get_options(int option_cnt, int *choice_cnt, int *defaults,
			    char ***option_choices);

/** A gui-dependent function called to set a global TEXT message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from within client_handle_server.  The message is a single
 *  string intended to be displayed as-is in a fixed-width font by the
 *  client.  A typical client will display most such messages in pop-up
 *  windows; however, those with special marks may have special
 *  significance.
 *  @param mark The "mark" ID tag string of the message.
 *  @param msg The message itself. */
extern void game_set_text_message(const char *mark, const char *msg);

/** A gui-dependent function called to set a global CARDLIST message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from within client_handle_server.  The message is a list
 *  of cards, an equal number for each player.
 *  @param mark The "mark" ID tag string of the message.
 *  @param length The number of cards in the list per player.
 *  @param cardlist The list of cards; cards are sorted by player.
 *  @see game_set_text_message
 */
extern void game_set_cardlist_message(const char *mark, int *lengths,
				      card_t ** cardlist);

/** A gui-dependent function called to set a player message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from handle_message_player.
 *  @param player The player number for which the message is intended.
 *  @param msg The message itself. */
extern void game_set_player_message(int player, const char *msg);

/** A gui-dependent, game-dependent function called to set a game message.
 *  This game message will comprise of "size" bytes of unread data that
 *  may be read in from the given socket so that the table can use
 *  game-specific extensions (which may allow for a better UI).
 *  @param fd The socket from which data may be read.
 *  @param game The type of game (which may or may not be useful).
 *  @param mark The "mark" of the message (which may or may not be useful).
 *  @param size The amount of data ready to be read.
 *  @return The number of bytes read, or negative for error.
 *  @note When in doubt, just use "return 0". */
extern int game_handle_game_message(int fd, int game_num, int size);

/** @} end of Callbacks */


/** @defgroup Responses
 *  The frontend must call these functions to send the appropriate
 *  responses to the server.
 *  @{ */

/** Sends a simple newgame response.
 *  @return 0 on success, -1 on failure.
 *  @see game_get_newgame */
int client_send_newgame(void);

/** Sends a bid response.
 *  @param bid The index of the bid chosen.
 *  @return 0 on success, -1 on failure
 *  @see game_get_bid */
int client_send_bid(int bid);

/** Sends an options response.
 *  @param option_cnt The number of options.
 *  @param options The choice made for each option.
 *  @return 0 on success, -1 on failure
 *  @see game_get_options */
int client_send_options(int option_cnt, int *options);

/** Sends a play response.
 *  @param card The card chosen to be played.
 *  @return 0 on success, -1 on failure
 *  @see game_get_play, game_alert_badplay */
int client_send_play(card_t card);

/** Sends a request for a sync.
 *  @return 0 on success, -1 on failure. */
int client_send_sync_request(void);

/** @} end of Responses */

#endif /* __COMMON_H__ */
