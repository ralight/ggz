/* 
 * File: client.h
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001 (as common.c)
 * Desc: Frontend to GGZCards Client-Common
 * $Id: client.h 8531 2006-08-21 19:34:47Z jdorje $
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#ifndef AI_CLIENT
#  define GUI_CLIENT
#endif

#include <ggz_common.h>
#include <ggz_dio.h>
#ifdef GUI_CLIENT
#  include <ggzmod.h>
#endif

#include "protocol.h"
#include "shared.h"

#ifndef __CLIENT_H__
#define __CLIENT_H__

/** @brief GGZ debugging type for client code. */
#define DBG_CLIENT "core"

/** @defgroup Player
 *  A "player" as seen by the client is really just a seat at the
 *  table, complete with a hand and everything.  It's not necessary
 *  that a player actually be sitting at that seat - there may just be
 *  a pile of cards.
 *  @note We are always player 0.  The server will deal with all conversions.
 *  @{ */

/** Contains all information about a seat at the table. */
typedef struct seat_t {
	GGZSeatType status;	/**< ggz seating assignment info */
	char *name;		/**< player's name */
	int ggzseat;		/**< GGZ seat number (or -1) */
	int team;		/**< The seat's team (or -1). */

	/** @brief Player's card on the table.
	 *
	 *  This variable is maintained by the core client-common code,
	 *  and should not be modified by the GUI table code.  It will
	 *  always hold the card known to be on the table for the
	 *  player, i.e. basically the same information that the
	 *  server knows (although with a bit of lag).  Note that this
	 *  need not correspond with what is actually drawn by the GUI.
	 */
	card_t table_card;
	int tricks;

	hand_t hand;		/**< player's hand */

	/** @brief The size of the uncompressed hand.
	 *
	 *  The player's "hand" is stored in the hand structure, above.
	 *  But for the convenience of certain GUI clients, separate
	 *  hand data is also tracked.  Here the u_hand_size is the
	 *  total number of entries in the u_hand array.  Each entry
	 *  in the u_hand array is marked as either valid or invalid.
	 *  Valid ones correspond to cards in the hand, invalid ones
	 *  are cards that have already been played.  (The "u" stands
	 *  for "uncollapsed", I suppose.) */
	int u_hand_size;

	bool bidding, playing;

	/** @see u_hand_size */
	struct {
		bool is_valid;
		card_t card;
	} *u_hand;
} seat_t;

typedef struct {
	score_data_t score;
	score_data_t *scores;
} team_t;

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

	int num_teams;		/**< The number of teams in the game. */
	team_t *teams;		/**< Data about each team */

	client_state_t state;	/**< The state the game is in */

	const char *gametype;	/**< String containing the game name. */
	int hand_num; /**< The number of the current hand (starts at 0) */

	char trump_suit;

	/** @brief The hand we're currently playing from.
	 *
	 *  This is the hand we're currently playing from, or -1 if
	 *  we're not currently playing.  Note when a play response
	 *  is sent to the server, it's not reset to -1 until
	 *  we get verification.
	 */
	int play_hand;
};

#ifdef GUI_CLIENT
GGZMod *client_get_ggzmod(void);
#endif

/** This is the game structure that contains all the common information
 *  about the ongoing game */
extern struct ggzcards_game_t ggzcards;

/** @} end of Game group */


/** @defgroup Setup
 *  Setup functions to be called by the frontend
 *  @{ */

/** This function should be called when the client first launches.  It
  * initializes all internal data and makes (or prepares to make) the
  * connection to the game server.
  * @note The socket FD will be returned separately via game_alert_server.
  * @return The FD of the connection ot the GGZ client.
  * @note AI clients don't have a GGZ connection; the return will be -1. */
int client_initialize(void);

/** This function should be called on any input from the GGZ fd.  It will
 *  do all the rest of the work, and call one of the callbacks if
 *  necessary.
 *  @see Callbacks */
int client_handle_ggz(void);

/** This function should be called on any input from the server fd.  It
 *  will do all the rest of the work, and call one of the callbacks if
 *  necessary.  AI clients need not worry about this function.
 *  @see Callbacks */
int client_handle_server(void);

/** This function should be called just before the client exits. */
void client_quit(void);

/** This function will return the file descriptor that is used to communicate
 *  with the server.
 */
GGZDataIO *client_get_dio(void);

/** @} end of Setup */


/** @defgroup Callbacks
 * Frontend-specific functions called by the common code.  It is
 * up to the frontend to define these functions appropriately.
 * @note These can easily be changed so that they'll fit better with other frontends.
 * @{ */

/** Handles an update of the server socket.  This will only happen once, when
 *  we first connect to the server. */
extern void game_alert_server(GGZDataIO * dio);

/** Handles a newgame request by calling client_send_newgame when
 *  ready (you may wish to ask the user first). */
extern void game_get_newgame(void);

/** Alerts the table to the start of a new game. */
extern void game_alert_newgame(cardset_type_t cardset_type);

/** Alerts the table to the start of a new hand. */
extern void game_alert_newhand(void);

/** Handles a gameover message.
 *  @param num_winners The number of players who won the game (0 or more)
 *  @param winners An array with the player numbers of the winners. */
extern void game_handle_gameover(int num_winners, int *winners);

/** Alerts the table of an update to the player data.  So that the
 *  table can tell what's changed, the old data is passed along.
 *  @param player The number of the player whose name we're talking about.
 *  @param old_status The previous type of the seat.
 *  @param old_name The previous name of the player (seat). */
extern void game_alert_player(int player,
			      GGZSeatType old_status,
			      const char *old_name);

/** Alert the table that the number of players has changed.  The
 *  table will probably want to redesign itself.
 *  @param new The new number of players.
 *  @param old The previous number of players.
 *  @note old and new will never be equal.
 *  @note ggzcards.num_players will have been updated. */
extern void game_alert_num_players(int new, int old);

/** Alert the table that scores data is available.  The table may then
 *  display or update scores.
 *  @param hand_num The hand number for which new scores are available. */
extern void game_alert_scores(int hand_num);

/** Alert the table that tricks data is available.  The table may then
 *  display or update tricks for each player. */
extern void game_alert_tricks_count(void);

/** Alerts the table to the maximum hand size.  There will never be more
 *  than this many cards in a hand (unless we send another alert). */
extern void game_alert_hand_size(int max_hand_size);

/** Called when the hand is changed; the frontend should draw/update it.
 *  @param player The player number of the player whose hand has changed. */
extern void game_display_hand(int player);

/** Called when players status (player.is_bidding and player.is_playing)
 *  are changed. */
extern void game_alert_players_status(void);

/** Called to request a bid.  The frontend should call client_send_bid at
  * any point afterwards to send the response.
  * @param possible_bids The number of possible bid choices.
  * @param bid_choices An array of strings, one corresponding to each bid choice. */
extern void game_get_bid(int possible_bids,
			 bid_t * bid_choices,
			 char **bid_texts, char **bid_descs);

/** Called to request a play.  The frontend should call client_send_play
 *  at any point afterwards to send the response.
 *  @param hand The player number of the hand to play from. */
extern void game_get_play(int play_hand,
			  int num_valid_cards, card_t * valid_cards);

/** Called when we're informed of a bid.  This data is not tracked
 *  internally and is of little use to the GUI (but would be crucial
 *  for a client-side AI).
 *  @param bidder The player who bid.
 *  @param bid The bid. */
extern void game_alert_bid(int bidder, bid_t bid);

/** Called when there's a bad play.  The frontend should display the
 *  error message, and call client_send_play again.  It may be necessary
 *  to redraw cards.
 *  @param err_msg The error message, as sent by the server. */
extern void game_alert_badplay(char *err_msg);

/** Called when we're informed of a play.  The hand itself has already
 *  been updated; the frontend should redraw it and perhaps draw animation.
 *  @param player The player whose hand was played from.
 *  @param card The card that was played.
 *  @param pos The (former) position of the card in a compressed hand.
 *  @param hand_pos The position of the card in an uncompressed hand. */
extern void game_alert_play(int player, card_t card,
			    int pos, int hand_pos);

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
extern int game_get_options(int option_cnt,
			    char **types,
			    char **descriptions,
			    int *choice_cnt,
			    int *defaults, char ***option_choices);

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
 *  @note When in doubt, leave function blank. */
extern void game_handle_game_message(GGZDataIO * dio, const char *game);

/** @} end of Callbacks */


/** @defgroup Responses
 *  The frontend must call these functions to send the appropriate
 *  responses to the server.
 *  @{ */

/** Sends the language to the server. */
void client_send_language(const char *lang);

/** Sends a simple newgame response.
 *  @see game_get_newgame */
void client_send_newgame(void);

/** Sends a bid response.
 *  @param bid The index of the bid chosen.
 *  @see game_get_bid */
void client_send_bid(int bid);

/** Sends an options response.
 *  @param option_cnt The number of options.
 *  @param options The choice made for each option.
 *  @see game_get_options */
void client_send_options(int option_cnt, int *options);

/** Sends a play response.
 *  @param card The card chosen to be played.
 *  @see game_get_play, game_alert_badplay */
void client_send_play(card_t card);

/** Sends a request for a sync. */
void client_send_sync_request(void);

/** @} end of Responses */

#endif /* __CLIENT_H__ */
