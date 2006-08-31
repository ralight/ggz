/* 
 * File: games/sueca.c
 * Author: Ismael Orenstein
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Sueca
 * $Id: sueca.c 8561 2006-08-31 08:00:24Z jdorje $
 *
 * Copyright (C) 2001-2002 Ismael Orenstein
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "game.h"
#include "message.h"
#include "score.h"
#include "team.h"

#include "sueca.h"

#define GSUECA ( *(sueca_game_t *)(game.specific) )
typedef struct sueca_game_t {
	int points_on_hand[4];
} sueca_game_t;

static bool sueca_is_valid_game(void);
static void sueca_deal_hand(void);
static card_t sueca_map_card(card_t c);
static void sueca_init_game(void);
static void sueca_start_bidding(void);
static void sueca_start_playing(void);
static void sueca_end_trick(void);
static void sueca_end_hand(void);
static void sueca_set_player_message(player_t p);

game_data_t sueca_data = {
	"sueca",
	N_("Sueca"),
	"http://www.pagat.com/aceten/sueca.html",
	sueca_is_valid_game,
	sueca_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	sueca_set_player_message,
	game_sync_player,
	game_get_bid_text,
	game_get_bid_desc,
	sueca_start_bidding,
	game_get_bid,
	game_handle_bid,
	game_next_bid,
	sueca_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	sueca_deal_hand,
	sueca_end_trick,
	sueca_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	sueca_map_card,
	game_compare_cards,
	game_send_hand
};


static bool sueca_is_valid_game(void)
{
	return (game.num_players == 4);
}

static void sueca_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(sueca_game_t));

	set_num_seats(game.num_players);
	set_num_teams(2);
	for (s = 0; s < game.num_seats; s++) {
		assign_seat(s, s);	/* one player per seat */
		assign_team(s % 2, s);
	}

	/* Target score is 4 */
	game.target_score = 4;
	/* Must break trump */
	game.must_break_trump = 1;

	/* Use the SUECA deck */
	game.deck_type = GGZ_DECK_SUECA;
	/* Point to the game rules */
}

static void sueca_start_bidding(void)
{
	/* there is no bidding phase */
	set_game_state(STATE_FIRST_TRICK);
}

static void sueca_start_playing(void)
{
	player_t p;
	game_start_playing();
	for (p = 0; p < game.num_players; p++)
		GSUECA.points_on_hand[p] = 0;
	set_all_player_messages();
}

static card_t sueca_map_card(card_t c)
{
	char n_face = c.face;
	/* This will make the sueca cards behave differently, so that we can
	   make game_end_trick work We will make the following mappings: 7 -> 
	   King King -> Queen Queen -> 10 */
	switch (c.face) {
	case 7:
		n_face = KING;
		break;
	case KING:
		n_face = QUEEN;
		break;
	case QUEEN:
		n_face = 10;
		break;
	}

	c.face = n_face;

	return c;

}

static void sueca_end_trick(void)
{
	player_t p;
	card_t card;
	int points = 0;
	game_end_trick();
	/* Get the value of this trick and add to the winner */
	for (p = 0; p < game.num_players; p++) {
		card = game.seats[game.players[p].seat].table;
		switch (card.face) {
		case ACE_HIGH:
			points += 11;
			break;
		case 7:
			points += 10;
			break;
		case KING:
			points += 4;
			break;
		case JACK:
			points += 3;
			break;
		case QUEEN:
			points += 2;
			break;
		}
	}
	GSUECA.points_on_hand[game.winner] += points;

	set_player_message(game.winner);
	set_player_message((game.winner + 2) % 4);

}

static void sueca_end_hand(void)
{
	int points;		/* The points for the 0/2 team */
	points = GSUECA.points_on_hand[0] + GSUECA.points_on_hand[2];
	/* TODO: Must add "dar uma bandeira" */
	if (points > 90) {
		change_score(0, 2);
		set_global_message("",
				   "%s/%s got more than 90 points and so got 2 games",
				   get_player_name(0), get_player_name(2));
	} else if (points > 60) {
		change_score(0, 1);
		set_global_message("",
				   "%s/%s got more than 60 points and won that hand",
				   get_player_name(0), get_player_name(2));
	} else if (points > 30) {
		change_score(1, 1);
		set_global_message("",
				   "%s/%s got more than 60 points and won that hand",
				   get_player_name(1), get_player_name(3));
	} else {
		change_score(1, 2);
		set_global_message("",
				   "%s/%s got more than 90 points and so got 2 games",
				   get_player_name(1), get_player_name(3));
	}

	set_all_player_messages();

}

static void sueca_deal_hand(void)
{
	seat_t s;
	game.hand_size = 10;
	for (s = 0; s < game.num_seats; s++)
		deal_hand(game.deck, game.hand_size, &game.seats[s].hand);
	set_trump_suit(game.seats[game.dealer].hand.cards[random() % 10].
		       suit);
	set_global_message("Trump", "Trump is %s.",
			   get_suit_name(game.trump));
	set_global_message("", "Trump is %s.", get_suit_name(game.trump));
}

static void sueca_set_player_message(player_t p)
{
	/* char rubber[4] = {' ', ' ', ' ', ' '} */
	int score_this_hand;
	seat_t s = game.players[p].seat;
	char status[] = "Playing";

	/* The player information should be like this: __Rubber_ ____X____
	   ____|____ _X--O--X_ ____|____ ____X____ This hand: %d %STATUS% */

	/* TODO: Wait till the client has fixed-width player messages */
	/* Get all the rubber values switch (game.players[p].score) { case 4:
	   rubber[3] = 'X'; case 3: rubber[2] = 'X'; case 2: rubber[1] = 'X';
	   case 1: rubber[0] = 'X'; } */

	score_this_hand =
	    GSUECA.points_on_hand[p] + GSUECA.points_on_hand[(p + 2) % 4];
	if (!
	    (game.state == STATE_WAIT_FOR_PLAY
	     && game.players[p].is_playing))
		sprintf(status, " ");
	put_player_message(s,
			   "Score:\nIn the game: %d\nIn the hand: %d\n%s",
			   get_player_score(p), score_this_hand, status);

	/* Waiting sprintf(message, " Rubber \n %c \n | \n %c--O--%c \n | \n
	   %c \nThis hand: %d\n%s", rubber[3], rubber[2], rubber[1],
	   rubber[0], score_this_hand, status); */



}
