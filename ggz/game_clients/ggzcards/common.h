/* $Id: common.h 2082 2001-07-23 23:38:03Z jdorje $ */
/*
 * File: common.h
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Frontend to GGZCards Client-Common
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

#ifndef __COMMON_H__
#define __COMMON_H__

/** A card.
 *  @todo Should the specific values be handled as enumerations?
 */
typedef struct card_t
{
#define UNKNOWN_FACE -1	/**< An unknown face on a card */
#define JACK 11		/**< A "jack" card face */
#define QUEEN 12	/**< A "queen" card face */
#define KING 13		/**< A "king" card face */
#define ACE_HIGH 14	/**< An "ace" (high) card face */
#define ACE_LOW 1	/**< An "ace" (low) card face */
#define JOKER1 0	/**< A "joker 1" card face */
#define JOKER2 1	/**< A "joker 2" card face */
	/** The face of the card.
	 *    - -1: unknown
	 *    - 1-13: A-K
	 *    - 14: A
	 *    - for "NO_SUIT" suit, 0-1 are jokers */
	char face;		/* -1: unknown, 1-13: A-K, 14: A
				 * for "none" suit, 0-1 are jokers */

#define UNKNOWN_SUIT -1
#define CLUBS 0
#define DIAMONDS 1
#define HEARTS 2
#define SPADES 3
#define NO_SUIT 4
	/** The suit of the card.
	 *    - -1: unknown
	 *    - 0-3: clubs, diamonds, hearts, spades
	 *    - 4: no suit */
	char suit;

#define UNKNOWN_DECK -1
	/** The deck number of the card.
	 *    - -1: unknown
	 *    - 0+: the deck number */
	char deck;
}
card_t;

/** An entirely unknown card. */
#define UNKNOWN_CARD (card_t){-1, -1, -1}


/** Hand structure.
 *  @todo Should this be merted into struct seat_t?
 */
struct hand_t
{
	int hand_size;		/**< the number of cards in the hand */
	card_t *card;		/**< the list of cards */
};

/** Contains all information about a seat at the table. */
typedef struct seat_t
{
	int seat;		/**< ggz seating assignment info */
	char *name;		/**< player's name */
	card_t table_card;	/**< card on table */
	struct hand_t hand;	/**< player's hand */
}
seat_t;

/** GGZCards client game states
 *  @note Any additional state data should be stored separately, while maintaining the state here. */
typedef enum
{
	WH_STATE_INIT,		/**< game hasn't started yet */
	WH_STATE_WAIT,		/**< waiting for others */
	WH_STATE_PLAY,		/**< our turn to play */
	WH_STATE_BID,		/**< our turn to bid */
	WH_STATE_DONE,		/**< game's over */
	WH_STATE_OPTIONS	/**< determining options */
}
client_state_t;

/** The game_t structure contains all global game data. */
struct game_t
{
	int num_players;	/**< The number of players in the game. */
	seat_t *players;	/**< Data about each player */
	client_state_t state;	/**< The state the game is in */
	int play_hand;		/**< The hand we're currently playing from */
	int max_hand_size;	/**< The maximum number of cards in a hand */
};

/** This is the game structure that contains all the common information
 *  about the ongoing game */
extern struct game_t game;


/** @defgroup Setup
 *  Setup functions to be called by the frontend
 *  @{ */

/** This function should be called when the client first launches.  It
  * initializes all internal data and makes the connection to the GGZ
  * client.
  * @return The file descriptor that is used to communicate with the server. */
int client_initialize();

/** This function should be called on any input from the server fd.  It
 *  will do all the rest of the work, and call one of the callbacks if
 *  necessary.
 *  @see Callbacks */
int client_handle_server();

/** This function should be called just before the client exits. */
void client_quit();

/** @} end of Setup */


/** Handles the debug message appropriately.
 *  @param fmt a printf-style format string.
 *  @param ... printf-style arguments. */
void client_debug(const char *fmt, ...);


/** @defgroup Callbacks
 * Frontend-specific functions called by the common code.
 * @{ */

/** Handles a newgame request by calling client_send_newgame when
 *  ready (you may wish to ask the user first). */
extern void table_get_newgame();

/** Handles a gameover message.
  * @param num_winners The number of players who won the game (0 or more)
  * @param winners An array with the player numbers of the winners. */
extern void table_handle_gameover(int num_winners, int *winners);

/** Alerts the table of a player's name, before changing that name in the game structure.
  * @param player The number of the player whose name we're talking about.
  * @param name The (possibly new) name of the player. */
extern void table_alert_player_name(int player, char *name);

/** Currently this "sets up the table".  It's called when the number
 *  of players or the max hand size changes.
 *  @todo This should be handled differently. */
extern void table_setup();

/** Currently this determines whether the max hand size is large enough.
 *  @return 1 if hand size is large enough, 0 if it's too small.
 *  @todo This should be handled differently. */
extern int table_verify_hand_size();

/** Called when the hand is changed; the frontend should draw/update it.
 *  @param player The player number of the player whose hand has changed. */
extern void table_display_hand(int player);

/** Called to request a bid.  The frontend should call client_send_bid at
  * any point afterwards to send the response.
  * @param possible_bids The number of possible bid choices.
  * @param bid_choices An array of strings, one corresponding to each bid choice. */
extern void table_get_bid(int possible_bids, char **bid_choices);

/** Called to request a play.  The frontend should call client_send_play
 *  at any point afterwards to send the response.
 *  @param hand The player number of the hand to play from. */
extern void table_get_play(int hand);

/** Called when there's a bad play.  The frontend should display the
 *  error message, and call client_send_play again.  It may be necessary
 *  to redraw cards.
 *  @param err_msg The error message, as sent by the server. */
extern void table_alert_badplay(char *err_msg);

/** Called when we're informed of a play.  The hand itself has already
 *  been updated; the frontend should redraw it and perhaps draw animation.
 *  @param player The player whose hand was played from.
 *  @param card The card that was played. */
extern void table_alert_play(int player, card_t card);

/** Called when we're informaed of the table cards.  The information
 *  itself resides in the player structures; all the frontend has to
 *  do is redraw the cards. */
extern void table_alert_table();

/** Called when we're informed of a trick being over.  All game
 *  information wil be updated; the frontend will have to clear
 *  the table cards from the table and alert the player to who
 *  won.
 *  @param player The player who won the trick. */
extern void table_alert_trick(int player);

/** Called to request options.  The frontend should ask the client
 *  what options are desired, and then call client_send_options.
 *  @param option_cnt The number of options.
 *  @param choice_cnt The number of choices for each option.
 *  @param defaults The default choice for each option.
 *  @param option_choices Text message for each choice of each option. */
extern void table_get_options(int option_cnt, int *choice_cnt, int *defaults,
			      char ***option_choices);

/** A gui-dependent function called to set the global message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from handle_message_global.
 *  @param mark The "mark" ID tag string of the message.
 *  @param msg The message itself. */
extern void table_set_global_message(const char *mark, const char *msg);

/** A gui-dependent function called to set a player message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from handle_message_player.
 *  @param player The player number for which the message is intended.
 *  @param msg The message itself. */
extern void table_set_player_message(int player, const char *msg);

/** @} end of Callbacks */


/** @defgroup Responses
 *  The frontend must call these functions to send the appropriate
 *  responses to the server.
 *  @{ */

/** Sends a simple newgame response.
 *  @return 0 on success, -1 on failure.
 *  @see table_get_newgame */
int client_send_newgame();

/** Sends a bid response.
 *  @param bid The index of the bid chosen.
 *  @return 0 on success, -1 on failure
 *  @see table_get_bid */
int client_send_bid(int bid);

/** Sends an options response.
 *  @param option_cnt The number of options.
 *  @param options The choice made for each option.
 *  @return 0 on success, -1 on failure
 *  @see table_get_options */
int client_send_options(int option_cnt, int *options);

/** Sends a play response.
 *  @param card The card chosen to be played.
 *  @return 0 on success, -1 on failure
 *  @see table_get_play, table_alert_badplay */
int client_send_play(card_t card);

/** Sends a request for a sync.
 *  @return 0 on success, -1 on failure. */
int client_send_sync_request();

/** @} end of Responses */

#endif /* __COMMON_H__ */
