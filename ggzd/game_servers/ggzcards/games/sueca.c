/*
 * File: games/sueca.c
 * Author: Ismael Orenstein
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Sueca
 *
 * Copyright (C) 2001 Ismael Orenstein
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
#include "../options.h"

#include "sueca.h"

static int sueca_is_valid_game();
static int sueca_deal_hand();
static card_t sueca_map_card(card_t c);
static void sueca_init_game();
static void sueca_start_bidding();
static void sueca_start_playing();
static void sueca_end_trick();
static void sueca_end_hand();
static void sueca_set_player_message(player_t p);

struct game_function_pointers sueca_funcs = {
	sueca_is_valid_game,
	sueca_init_game,
	game_get_options,
	game_handle_option,
	sueca_set_player_message,
	game_get_bid_text,
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


static int sueca_is_valid_game()
{
	return (game.num_players == 4);
}

static void sueca_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(sueca_game_t));
	set_num_seats(game.num_players);
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	/* sueca has no bidding */
	game.max_bid_choices = 0;
	game.max_bid_length = 0;
  /* Target score is 4 */
	game.target_score = 4;
  /* Must break trump */
  game.must_break_trump = 1;
  game.ai_type = GGZ_AI_RANDOM;
  /* Use the SUECA deck */
  game.deck_type = GGZ_DECK_SUECA;
  /* Everyone starts with 0 points in hand */
  GSUECA.points_on_hand[0] = 0;
  GSUECA.points_on_hand[1] = 0;
  GSUECA.points_on_hand[2] = 0;
  GSUECA.points_on_hand[3] = 0;

}

static void sueca_start_bidding()
{
	/* there is no bidding phase */
	set_game_state( WH_STATE_FIRST_TRICK );
}

static void sueca_start_playing() {
  player_t p;
  game_start_playing();
  for (p=0; p<game.num_players; p++)
    GSUECA.points_on_hand[p] = 0;
  set_all_player_messages();
}

static card_t sueca_map_card(card_t c)
{
  /* This will make the sueca cards behave differently, so that we can
   * make game_end_trick work
   * We will make the following mappings:
   * 7 -> King
   * King -> Queen
   * Queen -> 10 */
  switch (c.face) {
    case 7:
      c.face = KING;
      break;
    case QUEEN:
      c.face = 10;
      break;
    case KING:
      c.face = QUEEN;
      break;
  }

  return c;

}

static void sueca_end_trick()
{
  player_t p;
  card_t card;
  int points = 0;
	game_end_trick();
  /* Get the value of this trick and add to the winner */
	for(p=0; p<game.num_players; p++) {
		card = game.seats[ game.players[p].seat ].table;
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

  set_all_player_messages();

}

static void sueca_end_hand()
{
  int points; /* The points for the 0/2 team */
  points = GSUECA.points_on_hand[0] + GSUECA.points_on_hand[2];
  /* TODO: Must add "dar uma bandeira" */
  if (points > 90) {
    game.players[0].score+=2;
    game.players[2].score+=2;
    set_global_message("", "%s/%s got more than 90 points and so got 2 games", ggz_seats[0].name, ggz_seats[2].name);
  } else if (points > 60) {
    game.players[0].score++;
    game.players[2].score++;
    set_global_message("", "%s/%s got more than 60 points and won that hand", ggz_seats[0].name, ggz_seats[2].name);
  } else if (points > 30) {
    game.players[1].score++;
    game.players[3].score++;
    set_global_message("", "%s/%s got more than 60 points and won that hand", ggz_seats[1].name, ggz_seats[3].name);
  } else {
    game.players[1].score+=2;
    game.players[3].score+=2;
    set_global_message("", "%s/%s got more than 90 points and so got 2 games", ggz_seats[1].name, ggz_seats[3].name);
  }

  set_all_player_messages();

}

static int sueca_deal_hand()
{
  seat_t s;
  game.hand_size = 10;
  for (s = 0; s < game.num_seats; s++)
    cards_deal_hand(game.hand_size, &game.seats[s].hand);
  game.trump = game.seats[ game.dealer ].hand.cards[9].suit;
  set_global_message("Trump", "%s", suit_names[(int)game.trump]);
  return 0;
}

static void sueca_set_player_message(player_t p)
{
  /*char rubber[4] = {' ', ' ', ' ', ' '}*/
  int score_this_hand;
  seat_t s = game.players[p].seat;
  char* message = game.seats[s].message;
  char status[] = "Playing";

  /* The player information should be like this:
   * __Rubber_
   * ____X____
   * ____|____
   * _X--O--X_
   * ____|____
   * ____X____
   * This hand: %d
   * %STATUS% */

  /* TODO: Wait till the client has fixed-width player messages */
  /* Get all the rubber values
  switch (game.players[p].score) {
    case 4:
      rubber[3] = 'X';
    case 3:
      rubber[2] = 'X';
    case 2:
      rubber[1] = 'X';
    case 1:
      rubber[0] = 'X';
  } */

  score_this_hand = GSUECA.points_on_hand[p] + GSUECA.points_on_hand[(p+2)%4];
  if (!(game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play))
    sprintf(status, " ");
  sprintf(message, "Score:\nIn the game: %d\nIn the hand: %d\n%s", game.players[p].score, score_this_hand, status);

  /* Waiting
   * sprintf(message, "  Rubber \n    %c    \n    |    \n %c--O--%c \n    |    \n    %c    \nThis hand: %d\n%s", rubber[3], rubber[2], rubber[1], rubber[0], score_this_hand, status); */



}
