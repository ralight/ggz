/*
 * File: games/hearts.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Hearts
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#include <easysock.h>

#include "../games.h"
#include "../game.h"
#include "../ggz.h"
#include "../message.h"

#include "hearts.h"

static void hearts_init_game();
static int hearts_handle_gameover();
static void hearts_start_bidding();
static void hearts_start_playing();
static char* hearts_verify_play(card_t card);
static void hearts_end_trick();
static void hearts_end_hand();

struct game_function_pointers hearts_funcs = {
	hearts_init_game,
	game_get_options,
	game_handle_option,
	game_set_player_message,
	game_get_bid_text,
	hearts_start_bidding,
	game_get_bid,
	game_handle_bid,
	game_next_bid,
	hearts_start_playing,
	hearts_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	game_deal_hand,
	hearts_end_trick,
	hearts_end_hand,
	game_start_game,
	game_test_for_gameover,
	hearts_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};

static void hearts_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(hearts_game_t));
	set_num_seats(game.num_players);
	game.trump = -1; /* no trump in hearts */
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	/* hearts has no bidding */
	game.max_bid_choices = 0;
	game.max_bid_length = 0;
	game.target_score = 100;
	game.name = "Hearts";
}

static int hearts_handle_gameover()
{
	player_t p;
	player_t winners[game.num_players];
	int winner_cnt = 0;


	/* in hearts, the low score wins */
	int lo_score = 100;	
	for (p=0; p<game.num_players; p++) {
		if (game.players[p].score < lo_score) {
			winner_cnt = 1;
			winners[0] = p;
			lo_score = game.players[p].score;
		} else if (game.players[p].score == lo_score) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}
	return send_gameover(winner_cnt, winners);
}

static void hearts_start_bidding()
{
	/* there is no bidding phase */
	set_game_state( WH_STATE_FIRST_TRICK );
}

static void hearts_start_playing()
{
	player_t p;
	char face;

	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	/* we track the points that will be earned this hand. */
	for (p=0; p<game.num_players; p++)
		GHEARTS.points_on_hand[p] = 0;

	/* in hearts, the lowest club leads first */
	game.leader = -1;
	/* TODO: what if we're playing with multiple decks? */
	for (face = 2; face <= ACE_HIGH; face++) {
		for (p=0; p<game.num_players; p++) {
			/* TODO: this only works because the cards are sorted this way */
			card_t card = game.seats[ game.players[p].seat ].hand.cards[0];
			if (card.suit == CLUBS && card.face == face)
				game.leader = p;
				}
		if (game.leader != -1) {
			GHEARTS.lead_card_face = face;
			break;
		}
	}

	if (game.leader == -1) {
		ggz_debug("SERVER BUG: nobody has a club.");
		game.leader = (game.dealer + 1) % game.num_players;
	}
}

static char* hearts_verify_play( card_t card)
{
	seat_t s = game.play_seat;

	/* the game hearts must be handled differently */
	if (game.next_play == game.leader) {
		/* the low club must lead on the first trick */
		if (game.trick_count == 0 && game.play_count == 0 &&
		    (card.suit != CLUBS || card.face != GHEARTS.lead_card_face) )
			return "You must lead the low club.";
		/* you can't lead *hearts* until they're broken */
		/* TODO: integrate this with must_break_trump */
		if (card.suit != HEARTS) return NULL;
		if (game.trump_broken) return NULL;
		if (cards_suit_in_hand(&game.seats[s].hand, HEARTS) == game.seats[s].hand.hand_size)
			/* their entire hand is hearts */
			return NULL;
		return "Hearts have not yet been broken.";
	}

	return game_verify_play(card);
}

static void hearts_end_trick()
{
	player_t p;
	game_end_trick();
	for(p=0; p<game.num_players; p++) {
		card_t card = game.seats[ game.players[p].seat ].table;
		int points = 0;
		if (card.suit == HEARTS)
			points = 1;
		if (card.suit == SPADES && card.face == QUEEN)
			points = 13;
		/* in hearts, it's not really "trump broken" but rather "points broken".
		 * however, it works about the same way. */
		if (points > 0)
			game.trump_broken = 1;
		GHEARTS.points_on_hand[game.winner] += points;
	}
}

static void hearts_end_hand()
{
	player_t p;
	for (p=0; p<game.num_players; p++) {
		int points = GHEARTS.points_on_hand[p];
		int score = (points == 26 ? -26 : points);
		/* if you take all 26 points you "shoot the moon" and earn -26 instead.
		 * TODO: option of giving everyone else 26.  It could be handled as a bid... */
		game.players[p].score += score;
		if (score == -26)
			set_global_message("", "%s shot the moon.", ggz_seats[p].name);
	}
}
