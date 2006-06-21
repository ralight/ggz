/* 
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: default game functions
 * $Id: game.h 8240 2006-06-21 15:35:15Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It now
 * contains the default game functions; that is, the set of game functions
 * that may be used as defaults.  Each game (in games/) will define its
 * own functions as well.
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "games.h"
#include "types.h"

/* Game-specific functions */

/** @brief Is this game valid?
 *  @return TRUE iff the game is valid with the current settings.
 *  @note Must be replaced by a game-specific function.
 */
bool game_is_valid_game(void);

/** @brief Initialize the game.
 *  @note Must be replaced by a game-specific function.
 */
void game_init_game(void);

/** @brief Set up game options
 *
 *  This function is called to set up the list of options for the game (it 
 *  (would perhaps be better named game_init_options).  The function 
 *  assembles the list of options using add_option.  Then the calling code 
 *  will automatically request the host player to select from these 
 *  options.  (The option set must match what is handled by 
 *  game_handle_option and game_get_option_text.)
 *
 *  By default, only one option is provided: playing with open hands.  If 
 *  chosen, this is supported automatically by the core code.
 *
 *  @see add_option
 */
void game_get_options(void);

/** @brief Handles a single option
 *
 *  After the host player has selected options, for each option selected 
 *  game_handle_option will be called to set that option.  (The option set 
 *  it can handle must match those handled by game_get_options and 
 *  game_get_option_text.)
 *
 *  By default, only one option is provided: playing with open hands.  If 
 *  chosen, this is supported automatically by the core code.
 *
 *  @param option The name (key string) of the option
 *  @param value The value the option takes
 *  @see game_get_options
 */
int game_handle_option(char *option, int value);

/** @brief Return a descriptive string for the option's value
 *
 *  After the game starts, all clients are sent a list of what options are 
 *  in use.  This list is automatically assembled by calling this function
 *  for each chosen option.
 *
 *  By default, only one option is provided: playing with open hands.  If 
 *  chosen, this is supported automatically by the core code.
 *
 *  @param buf The buffer in which to place the string
 *  @param buf_len The size of the buffer
 *  @param option The name (key string) of the option
 *  @param value The value we want a description for
 *  @return A usable pointer to the buffer
 *  @see game_get_options
 */
char *game_get_option_text(char *buf, int buf_len, char *option, int value);

/** @brief Set the player message for the given player.
 *
 *  This function should use the messaging functions to create a player 
 *  message.  This text message is automatically passed to the clients by 
 *  the core code.  By default, a simple message is assembled which includes
 *  the player's score, bid, tricks taken, and current action
 *  (bidding/playing/neither).
 *
 *  @param player The player for which to create a message
 *  @see message.h
 */
void game_set_player_message(player_t player);

/** @brief Send game-specific synchronization to the player.
 *
 *  This function is called when a player sync is invoked; for instance,
 *  when a player joins the table.  The player may be a regular seat or
 *  a spectator.  It should send any game-specific synchronization data
 *  to the client (usually in the form of a specialized game packet).
 *
 *  @param player The player to be synchronized.
 */
void game_sync_player(player_t p);

/** @brief Set the text message for the given bid.
 *
 *  Creates a text description of the bid, placing it into the buffer.  This
 *  text is what is shown to clients when asking for or telling about a bid.
 *
 *  @param buf A buffer in which to put the text.
 *  @param buf_len The length of the buffer.
 *  @param bid The bid for which to create a message.
 *  @return The length of the text.
 *  @note Any game that uses bidding must replace this function.
 */
int game_get_bid_text(char *buf, size_t buf_len, bid_t bid);

/** @brief Set a descriptive text for the given bid.
 *
 *  Creates a more lenghty text description of the bid, placing it into
 *  the buffer.  This text may be shown to players on demand when they
 *  bid (for instance, as tooltips) or for other purposes (as yet
 *  undetermined).
 *
 *  The default behavior is to just call get_bid_text.
 *
 *  @param buf A buffer in which to put the text.
 *  @param buf_len The length of the buffer.
 *  @param bid The bid for which to create a message.
 *  @return The length of the text.
 *  @see game_get_bid_text
 */
int game_get_bid_desc(char *buf, size_t buf_len, bid_t bid);

/** @brief Called at the beginning of the bidding sequence.
 *
 *  This function must do any special handling necessary before beginning
 *  bidding.  By default, we set up the number of bids to equal the number
 *  of players, and the player to the dealers left as the first bidder.
 *
 *  @note Any game which doesn't use bidding must replace this function.
 */
void game_start_bidding(void);

/** @brief Get the next bid.
 *
 *  This function is called by the core code to request the next bid.  The
 *  function should assemble a list of bids using the bid.h interface, then
 *  call req_bid to request a bid of the client.  By default, the function
 *  doesn't do anything at all.
 *
 *  @return 0 on (network) success, -1 on error.
 *  @todo The calling code should call req_bid, not the function itself.
 *  @note Any game that uses bidding must replace this function.
 */
void game_get_bid(void);

/** @brief Specialty handling of a bid.
 *
 *  Any time we receive a bid from the client, this function is called to
 *  do any specialy handling.  By default, nothing is done.  In fact, many
 *  games will not need to do any special handling.  The calling code already
 *  records the players bid, changes the player's message (which should then
 *  reflect the bid), and increments the bid counter.
 *
 *  @param p The player who has bid.
 *  @param bid The bid they have made.
 */
void game_handle_bid(player_t p, bid_t bid);

/** @brief Specialty handling to prep for next bid.
 *
 *  This function is called by the core code after each bid is made and
 *  handled.  It should change any state data to prepare for the next bid.
 *  The calling code will already have incremented game.bid_count.  By
 *  default, this function moves game.next_bid to point to the next
 *  player (to the left).  For games that use simple bidding systems, this
 *  should be sufficient.
 */
void game_next_bid(void);

/** @brief Specialty handling before starting to play a hand.
 *
 *  This function is called by the core code when we first start playing
 *  a hand (after bidding has completed and before the first play).  By
 *  default, we set the trick total (total number of tricks on the hand)
 *  to be equal to the hand size, set the play total (total number of
 *  plays per trick) to be equal to the number of players, and set the
 *  player to the dealer's left to lead on the first trick.  For many
 *  games this should be sufficient.
 *
 *  @see bridge_start_playing
 */
void game_start_playing(void);

/** @brief Check the validity of a play.
 *
 *  This function determines if a player's play is valid.  The core code
 *  calls this to check the play before accepting the play.  By default, we
 *  only force plays to follow suit.  We also support must_overtrump and
 *  must_break_trump, if those are set.  We call map_card on the card, so
 *  that the code will work for games that treat cards like those of
 *  another suit.  In short, this should work for almost all games.
 *
 *  If you do replace this function, you'll probably want to call it anyway
 *  to do the bulk of the work.  Make sure you pass it only unmapped cards,
 *  so that no card is mapped twice.
 *
 *  @return NULL if the play is valid, an error message if not.
 *  @see game_map_card
 *  @see hearts_verify_play
 */
char *game_verify_play(player_t p, card_t card);

/** @brief Prep for the next play.
 *
 *  This function is called by the core code after each play is accepted.  It
 *  should set up the next play.  By default, we just have play move to the
 *  left (by setting next_play).  The core code also does a lot of prep work:
 *  it increments play_count (before calling this function), and will stop
 *  play once we reach play_total plays (after calling this function).  This
 *  is sufficient for every game I know of.
 */
void game_next_play(void);

/** @brief Get the next play.
 *
 *  The core code will call this function to get game.next_play player to
 *  play.  The function should call req_play to actually request the play
 *  of the player.  By default, we just have _that_ player play from _their_
 *  hand.  This should be sufficient for most games.
 *
 *  @param p The player who is to play.
 *  @see bridge_get_play
 */
void game_get_play(player_t p);

/** @brief Specialty handling of a played card.
 *
 *  This function is called to handle every accepted play.  The core code also
 *  does a lot of handling of the play already.  It will mark game.trump_broken
 *  and game.lead_card, if appropriate.  It will remove the card from the hand,
 *  of course.  It will  put the card on the table, and reset the player's message.
 *  All this function does is broadcast the play to everyone.
 *
 *  @param p The player who played.
 *  @param s The seat they played from.
 *  @param c The card that was played.
 *  @see bridge_handle_play
 */
void game_handle_play(player_t p, seat_t s, card_t c);

/** @brief Deal a hand.
 *
 *  This function deals a hand, placing cards directly into the players'
 *  hands.  By default, it sets game.hand_size to be the maximum possible,
 *  and deals that many cards into everyone's hand.
 *
 *  @see cards_deal_hand
 *  @see lapocha_deal_hand
 *  @see spades_deal_hand
 */
void game_deal_hand(void);

/** @brief Specialty handling for the end of a trick.
 *
 *  This function is called by the core code at the end of a trick, to do
 *  any specialty handling.  By default, we determine the winning player
 *  using standard rules (the highest trump played wins, otherwise the
 *  highest card of the suit lead wins), increment that player's trick
 *  count, set that player to lead the next trick (game.leader).  We also
 *  mark that player as the winner (game.winner), and reset their player
 *  message and that of any of their teammates.
 *
 *  @todo We should update teammates' messages automatically.
 *  @see hearts_end_trick
 */
void game_end_trick(void);

/** @brief Specialty handling for the end of a hand.
 *
 *  This function is called to do handling at the end of a hand.  By
 *  default, we do nothing.  Most games will at least want to update the
 *  score here.
 *
 *  @note Must be replaced by a game-specific function.
 *  @see spades_end_hand
 */
void game_end_hand(void);

/** @brief Specialty handling before the start of a game.
 *
 *  This function is called by the core code at the start of a game.  It
 *  should reset all necessary data for the new game.  By default, we just
 *  reset all players' scores to 0.  The calling code will also do some
 *  necessary things, including resetting all players' messages.  This should
 *  be sufficient for many games.
 *
 *  @see spades_start_game
 */
void game_start_game(void);

/** @brief Check to see if the game should be over.
 *  This function is called at the end of every hand to determine if the
 *  current game is over.  By default, it just checks every players' score
 *  to see if they have reached game.target_score.  This will be sufficient
 *  for all games that are played to points.
 *
 *  @return TRUE if the game is over, FALSE if not.
 *  @see lapocha_test_for_gameover
 */
bool game_test_for_gameover(void);

/** @brief Specialty handling when a game is over.
 *
 *  This function is called when the game is actually over.  It should
 *  determine who has won, and call handle_gameover_event to alert the
 *  clients that  the game is over.  By default, we just take the player(s)
 *  with the highest score(s) as the winner(s).  This should be sufficient
 *  for many games.
 *
 *  @return 0 on success, -1 on (network) failure
 *  @see lapocha_handle_gameover
 */
void game_handle_gameover(void);

/** @brief Makes one card act like another.
 *
 *  This makes a card behave entirely as if it were a different card.  It is
 *  called by the core code in a number of places before doing operations on
 *  a card.  By default, no cards are mapped (the function just returns the
 *  card directly), which should be sufficient for most games.
 * 
 *  @param card the card that is being mapped
 *  @return the card it is mapped to
 *  @see euchre_map_card
 */
card_t game_map_card(card_t card);

/** @brief Compares two cards for hand-sorting purposes.
 *
 *  When sorting hands, this function is called to compare the cards.  By
 *  default, it just sorts primarily by suit then by face.  This is
 *  sufficient for most games.
 *  
 *  @return -1, 0, 1 if card1 is lower, equal, or higher than card2
 *  @note The cards passed in will already have been mapped as necessary.
 *  @see bridge_compare_cards
 */
int game_compare_cards(card_t card1, card_t card2);

/** @brief Sends a hand to a player.
 *
 *  This function is called to send any hand to a player.  The function should
 *  determine whether or not the player gets to see the hand, and call
 *  send_hand to send the hand (or do it manually if necessary).  By default,
 *  we only show players their own hands.
 * 
 *  @param p the player the hand is being sent to
 *  @param s the seat of the hand being shown
 *  @return 0 on success, -1 on (network) failure.
 *  @note send_hand will take care of game.open_hands.
 *  @see bridge_send_hand
 */
void game_send_hand(player_t p, seat_t s);
