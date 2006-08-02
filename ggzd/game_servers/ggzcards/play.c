/*
 * File: play.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Functions and data for playing system
 * $Id: play.c 8454 2006-08-02 01:47:14Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>

#include "common.h"
#include "message.h"
#include "net.h"
#include "play.h"
#include "score.h"

#ifndef NDEBUG
static bool hand_has_valid_card(player_t p, hand_t *hand);
#endif /* NDEBUG */

bool is_anyone_playing(void)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].is_playing)
			return TRUE;
	return FALSE;
}

void request_client_play(player_t p, seat_t s)
{
#ifndef NDEBUG
	/* Sanity checks */
	player_t p2;
	assert(!game.players[p].is_playing);
	for (p2 = 0; p2 < game.num_players; p2++)
		if (game.players[p2].is_playing)
			assert(game.players[p2].play_seat != s);
#endif
	
	/* although the game_* functions probably track this data themselves,
	   we track it here as well just in case. */
	game.players[p].is_playing = TRUE;
	game.players[p].play_seat = s;

	set_game_state(STATE_WAIT_FOR_PLAY);
	set_player_message(p);
	
	net_send_play_request(p, s);
}

#ifndef NDEBUG
static bool hand_has_valid_card(player_t p, hand_t *hand)
{
	int i;

	for (i = 0; i < hand->hand_size; i++)
		if (game.data->verify_play(p, hand->cards[i]) == NULL)
			return TRUE;
			
	return FALSE;
}
#endif /* NDEBUG */

void handle_client_play(player_t p, card_t card)
{
	hand_t *hand;
	int i;
	char *err;

	/* Only players and bots can play. */
	if (!IS_REAL_PLAYER(p))
		return;

	/* Is is this player's turn to play? */
	if (!game.players[p].is_playing) {
		/* better to just ignore it; a MSG_BADPLAY requests a new
		   play */
		ggz_debug(DBG_CLIENT,
			  "player %d/%s played out of turn!?!?",
			  p, get_player_name(p));
		return;
	}

	/* find the card played */
	hand = &game.seats[game.players[p].play_seat].hand;
	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			break;

	if (i == hand->hand_size) {
		/* They tried to play a card that wasn't in their hand.  This
		   is most likely just a bug on the client's part.  Hopefully
		   sending a sync will fix it (it will also re-request the
		   play). */
		send_sync(p);
		ggz_debug(DBG_PLAY, "CLIENT BUG: "
			    "player %d/%s played a card that wasn't "
			    "in their hand.",
			    p, get_player_name(p));
		return;
	}

	ggz_debug(DBG_PLAY, "We received a play of card "
		    "(%d %d %d) from player %d/%s.", card.face, card.suit,
		    card.deck, p, get_player_name(p));

	/* we've verified that this card could have physically been played;
	   we still need to check if it's a legal play Note, however, that we
	   don't return -1 on an error here.  An error returned indicates a
	   GGZ error, which is not what happened.  This is just a player
	   mistake */
	err = game.data->verify_play(p, card);
	if (err == NULL)
		/* any AI routine would also call handle_play_event, so the
		   ai must also check the validity as above.  This could be
		   changed... */
		handle_play_event(p, card);
	else {
		/* Sending the badplay message should alert the client to
		   try again. */
		assert(game.state == STATE_WAIT_FOR_PLAY);
		net_send_badplay(p, err);
		assert(hand_has_valid_card(p, hand));
	}
}

/* This handles the event of someone playing a card.  The caller must have
   already checked whether the card is in the seat's hand. */
void handle_play_event(player_t p, card_t card)
{
	seat_t s = game.players[p].play_seat;
	int i;
	hand_t *hand = &game.seats[s].hand;
	
	ggz_debug(DBG_PLAY, "%s played the %s of %s.",
	            get_player_name(p),
	            get_face_name(card.face),
	            get_suit_name(card.suit));
	
	/* is this the right place for this? */
	assert(game.players[p].is_playing);
	game.players[p].is_playing = FALSE;

	/* remove the card from the player's hand by sliding it to the end. */
	/* TODO: this is quite inefficient */
	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			break;
	assert(i < hand->hand_size);
	for (; i < hand->hand_size - 1; i++)
		hand->cards[i] = hand->cards[i + 1];
	hand->hand_size--;
	hand->cards[hand->hand_size] = card;

	/* Move the card onto the table */
	game.seats[s].table = card;
	if (game.next_play == game.leader)
		game.lead_card = card;
	
	/* Increment the play_count.  Note that this must be done
	   *after* handle_play is called (above), but before we
	   check for still_playing (below). */
	game.play_count++;

	/* Set trump_broken (if it was) */
	if (card.suit == game.trump)
		game.trump_broken = TRUE;

	/* do game-specific handling */
	game.data->handle_play(p, s, card);

	/* this is the player that just finished playing */
	set_player_message(p);
	
	/* If we're still playing, wait for the current round of
	   plays to finish.  Circumventing this process would be
	   tricky; it would have to be handled in game->handle_play. */
	if (is_anyone_playing()) {
		assert(game.state == STATE_WAIT_FOR_PLAY);
		return;
	}

	/* set up next move */
	game.data->next_play();
	if (game.play_count < game.play_total)
		set_game_state(STATE_NEXT_PLAY);
	else {
		/* end of trick */
		ggz_debug(DBG_PLAY, "End of trick; %d/%d.  Scoring it.",
			    game.trick_count, game.trick_total);
		assert(game.play_count == game.play_total);
		game.data->end_trick();
		send_last_trick();
		handle_trick_event(game.winner);
		game.trick_count++;
		set_game_state(STATE_NEXT_TRICK);
		if (game.trick_count == game.trick_total) {
			/* end of the hand */
			ggz_debug(DBG_PLAY, "End of hand number %d.",
				    game.hand_num);
			send_last_hand();
			game.data->end_hand();
			set_all_player_messages();
			update_cumulative_scores();
			game.dealer = (game.dealer + 1) % game.num_players;
			game.hand_num++;
			set_game_state(STATE_NEXT_HAND);
		}
	}

	/* do next move */
	next_move();
}
