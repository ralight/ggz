/*
 * File: games/suaro.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Game-dependent game functions for Suaro
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

#include "suaro.h"

static void suaro_init_game();
static int suaro_get_options();
static void suaro_handle_options(int *options);
static void suaro_start_bidding();
static int suaro_get_bid();
static int suaro_handle_bid(bid_t bid);
static void suaro_next_bid();
static void suaro_start_playing(void);
static int suaro_deal_hand();
static int suaro_send_hand(player_t p, seat_t s);
static int suaro_get_bid_text(char* buf, int buf_len, bid_t bid);
static void suaro_end_hand();
static void suaro_set_player_message(player_t p);
static void suaro_end_trick();

struct game_function_pointers suaro_funcs = {
	suaro_init_game,
	suaro_get_options,
	suaro_handle_options,
	suaro_set_player_message,
	suaro_get_bid_text,
	suaro_start_bidding,
	suaro_get_bid,
	suaro_handle_bid,
	suaro_next_bid,
	suaro_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	suaro_deal_hand,
	suaro_end_trick,
	suaro_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	game_compare_cards,
	suaro_send_hand
};


 /* these should be low, clubs, diamonds, ..., high, but that won't fit in the client window */
static char* short_suaro_suit_names[6] = {"lo", "C", "D", "H", "S", "hi"};
static char* long_suaro_suit_names[6] = {"low", "clubs", "diamonds", "hearts", "spades", "high"};

static void suaro_init_game()
{
	static struct ggz_seat_t ggz[2] = { {GGZ_SEAT_NONE, "Kitty", -1},
					    {GGZ_SEAT_NONE, "Up-Card", -1} };
	game.specific = alloc(sizeof(suaro_game_t));
	set_num_seats(4);
	game.seats[0].ggz = &ggz_seats[0];
	game.players[0].seat = 0;
	game.seats[2].ggz = &ggz_seats[1];
	game.players[1].seat = 2;
	game.seats[1].ggz = &ggz[0];
	game.seats[3].ggz = &ggz[1];
	/* most possible bids for suaro: 6 suits * 5 numbers + pass + double = 32
	 * for shotgun suaro, that becomes 62 possible bids
	 * longest possible bid: 9 diamonds = 11
	 * for shotgun suaro that becomes K 9 diamonds = 13 */
	game.deck_type = GGZ_DECK_SUARO;
	game.max_bid_choices = 62;
	game.max_bid_length = 13;
	game.max_hand_length = 9;
	game.name = "Suaro";
	game.rules_url = "http://suaro.dhs.org/";
	game.target_score = 50;
	SUARO.shotgun = 1;	/* shotgun suaro */
	SUARO.declarer = -1;
	game.last_trick = 1;	/* show "last trick" */
}

static int suaro_get_options()
{
        int fd;
	fd = ggz_seats[game.host].fd;
	if (fd == -1) {
		ggz_debug("SERVER BUG: nonexistent host.");
		return -1;
	}
  			/* four options for now:
			 *   shotgun -> boolean
			 *   unlimited redoubling -> boolean
			 *   persistent doubles -> boolean
			 *   target score -> 25, 50, 100, 200, 500, 1000
			 */
			game.num_options = 4;
			if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
				es_write_int(fd, game.num_options) < 0 || /* number of options */
				es_write_int(fd, 1) < 0 || /* first option: 1 choice */
				es_write_int(fd, 1) < 0 || /* first option: default is 1 */
				es_write_string(fd, "shotgun") < 0 || /* the first option is "shotgun" */
				es_write_int(fd, 1) < 0 || /* second option: 1 choice */
				es_write_int(fd, 0) < 0 || /* second option: default is 0 */
				es_write_string(fd, "unlimited redoubling") < 0 ||
				es_write_int(fd, 1) < 0 || /* third option: 1 choice */
				es_write_int(fd, 0) < 0 || /* third option: default is 0 */
				es_write_string(fd, "persistent doubles") < 0 ||
				es_write_int(fd, 6) < 0 || /* fourth option: 6 choices */
				es_write_int(fd, 1) < 0 || /* fourth option: default is 1 (on a scale from 0-2) */
				es_write_string(fd, "Game to 25") < 0 ||
				es_write_string(fd, "Game to 50") < 0 ||
				es_write_string(fd, "Game to 100") < 0 ||
				es_write_string(fd, "Game to 200") < 0 ||
				es_write_string(fd, "Game to 500") < 0 ||
				es_write_string(fd, "Game to 1000") < 0
			   )
				return -1;
	return 0;
}

static void suaro_handle_options(int *options)
{
			if (options[0] >= 0) SUARO.shotgun = options[0];
			if (options[1] >= 0) SUARO.unlimited_redoubling = options[1];
			if (options[2] >= 0) SUARO.persistent_doubles = options[2];
			switch (options[3]) {
				/* this highlights a problem with this protocol.  Only discrete entries are possible.
				 * it would be better if the client could just enter a number */
				case 0: game.target_score = 25; break;
				case 1: game.target_score = 50; break;
				case 2: game.target_score = 100; break;
				case 3: game.target_score = 200; break;
				case 4: game.target_score = 500; break;
				case 5: game.target_score = 1000; break;
				default: break;
			}
			set_global_message("Options",
				"%s%s%sFirst player to %d points wins.",
				SUARO.shotgun ? "Shotgun rules are in effect.\n" : "",
				SUARO.unlimited_redoubling ? "Unlimited redoubling is allowed.\n" : "",
				SUARO.persistent_doubles ? "Doubles are persistent.\n" : "",
				game.target_score);
			game.options_initted = 1; /* we could do another round of option requests, if we wanted */
}

static void suaro_start_bidding()
{
	char suit;
 			game.bid_total = -1; /* no set total */

			/* key card determines first bidder */
			suit = game.seats[3].hand.cards[0].suit;
			game.next_bid = (suit == CLUBS || suit == SPADES) ? 0 : 1;

			SUARO.pass_count = 0;
			SUARO.bonus = 1;
}

static int suaro_get_bid()
{
	int status = 0, index=0;
	bid_t bid;
	bid.bid = 0;

			/* in suaro, a bid consists of a number and a suit. */

			/* make a list of regular bids */
			for(bid.sbid.val = 5; bid.sbid.val <= 9; bid.sbid.val++) {
				for(bid.sbid.suit = SUARO_LOW; bid.sbid.suit <= SUARO_HIGH; bid.sbid.suit++) {
					if (bid.sbid.val < SUARO.contract) continue;
					if (bid.sbid.val == SUARO.contract && bid.sbid.suit <= SUARO.contract_suit) continue;
					game.bid_choices[index] = bid;
					index++;
					if (SUARO.shotgun) {
						/* in "shotgun" suaro, you are allowed to bid on the kitty just like on your hand! */
						bid_t kbid = bid;
						kbid.sbid.spec = SUARO_KITTY;
						game.bid_choices[index] = kbid;
						index++;
					}
				}
			}

			/* make "double" or "redouble" bid */
			if ( SUARO.contract > 0 && SUARO.bonus == 1) {
				/* unless unlimited doubling is specifically allowed,
				 * only double and redouble are possible */
				bid.bid = 0;
				bid.sbid.spec = SUARO_DOUBLE;
				game.bid_choices[index] = bid;
				index++;
			} else if (SUARO.contract > 0 &&
			           (SUARO.bonus < 4 || SUARO.unlimited_redoubling)) {
				bid.bid = 0;
				bid.sbid.spec = SUARO_REDOUBLE;
				game.bid_choices[index] = bid;
				index++;
			}

			/* make "pass" bid */
			bid.bid = 0;
			bid.sbid.spec = SUARO_PASS;
			game.bid_choices[index] = bid;
			index++;

			status = req_bid(game.next_bid, index, NULL);

	return status;
}

static int suaro_handle_bid(bid_t bid)
{
			if (bid.sbid.spec == SUARO_PASS) {
				SUARO.pass_count++;
			} else if (bid.sbid.spec == SUARO_DOUBLE ||
				   bid.sbid.spec == SUARO_REDOUBLE) {
				SUARO.pass_count = 1; /* one more pass will end it */
				SUARO.bonus *= 2;
			} else {
				SUARO.declarer = game.next_bid;
				SUARO.pass_count = 1; /* one more pass will end it */
				if (!SUARO.persistent_doubles)
					SUARO.bonus = 1; /* reset any doubles */
				SUARO.contract = bid.sbid.val;
				SUARO.kitty = (bid.sbid.spec == SUARO_KITTY);
				SUARO.contract_suit = bid.sbid.suit;
				if (bid.sbid.suit > SUARO_LOW && bid.sbid.suit < SUARO_HIGH)
					game.trump = bid.sbid.suit - 1; /* a minor hack */
				else
					game.trump = -1;
			}	
	return 0;
}

static void suaro_next_bid()
{
			if (SUARO.pass_count == 2) {
				/* done bidding */
				if (SUARO.contract == 0) {
					ggz_debug("Two passes; redealing hand.");
					set_global_message("", "%s", "Everyone passed; redealing.");
					set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
				} else {
					ggz_debug("A pass; bidding is over.");
					game.bid_total = game.bid_count;
					/* contract was determined in game_handle_bid */
				}
			} else
				game_next_bid(); /* TODO */
}

static void suaro_start_playing(void)
{
	player_t p;

	game.trick_total = game.hand_size;
	game.play_total = game.num_players;
			/* declarer is set in game_handle_bid */
			set_global_message("", "%s has the contract at %s%d %s%s.",
				ggz_seats[SUARO.declarer].name,
				SUARO.kitty ? "kitty " : "",
				SUARO.contract, long_suaro_suit_names[(int)SUARO.contract_suit],
				SUARO.bonus == 1 ? "" : SUARO.bonus == 2 ? ", doubled" : ", redoubled");
			game.leader = 1 - SUARO.declarer;
			if (SUARO.kitty) {
				/* if it's a kitty bid, the declarer takes up the kitty and
				 * lays their own hand down (face up) in its place */
				card_t* temp;
				seat_t s1, s2;
				s1 = 1; /* kitty hand */
				s2 = game.players[SUARO.declarer].seat;
				/* this "swapping" only works because the cards in the hands
				 * are specifically allocated by malloc.  Also note that there
				 * are always 9 cards in both hands. */
				temp = game.seats[s1].hand.cards;
				game.seats[s1].hand.cards = game.seats[s2].hand.cards;
				game.seats[s2].hand.cards = temp;
				send_hand(SUARO.declarer, s2, 1); /* reveal the new hand to the player */
				SUARO.kitty_revealed = 1;
				for(p = 0; p<game.num_players; p++)
					/* reveal the kitty to everyone */
					send_hand(p, s1, 1);
			}	
}

static int suaro_deal_hand()
{

	seat_t s;
	int result=0;
			/* in suaro, players 0 and 1 (seats 0 and 2) get 9 cards each.
			 * the kitty (seat 1) also gets 9 cards, and the face card
			 * (seat 3) is just one card. */
			game.hand_size = 9;
			for(s = 0; s < 3; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
			cards_deal_hand(1, &game.seats[3].hand);	
	return result;
}

static int suaro_send_hand(player_t p, seat_t s)
{
 			/* reveal the kitty after it's been turned up */
			if (s == 1 && SUARO.kitty_revealed)
				return send_hand(p, s, 1);
			/* each player can see their own hand plus the
			 * key card */
			return send_hand(p, s,
				game.players[p].seat == s || s == 3);
}

static int suaro_get_bid_text(char* buf, int buf_len, bid_t bid)
{
 			if (bid.sbid.spec == SUARO_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == SUARO_DOUBLE) return snprintf(buf, buf_len, "Double");
			if (bid.sbid.spec == SUARO_REDOUBLE) return snprintf(buf, buf_len, "Redouble");
			if (bid.sbid.val > 0) return snprintf(buf, buf_len, "%s%d %s", (bid.sbid.spec == SUARO_KITTY) ? "K " : "", bid.sbid.val, short_suaro_suit_names[(int)bid.sbid.suit]);

	return snprintf(buf, buf_len, "%s", "");
}

static void suaro_end_hand()
{
            			int points, tricks;
			player_t winner;

	game_end_hand();
			tricks = game.players[SUARO.declarer].tricks;
			if (tricks >= SUARO.contract) {
				int overtricks = tricks - SUARO.contract;
				winner = SUARO.declarer;
				/* you get the value of the contract MINUS
				 * the number of overtricks == 2 * contract - tricks */
				points = (SUARO.contract - overtricks) * SUARO.bonus;
				ggz_debug("Player %d/%s made their bid of %d, plus %d overtricks for %d points.",
					  winner, ggz_seats[winner].name, SUARO.contract, overtricks, points);
			} else {
				winner = 1-SUARO.declarer;
				/* for setting, you just get the value of the contract */
				points = SUARO.contract * SUARO.bonus;
				ggz_debug("Player %d/%s set the bid of %d, earning %d points.",
					  winner, ggz_seats[winner].name, SUARO.contract, points);
			}
			set_global_message("", "%s %s the bid and earned %d point%s.",
					ggz_seats[winner].name,
					winner == SUARO.declarer ? "made" : "set",
					points,
					points == 1 ? "" : "s");
			game.players[winner].score += points;
			SUARO.declarer = -1;
			SUARO.kitty_revealed = 0;
			SUARO.contract = 0;

}

static void suaro_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks);
			if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
				if (p == SUARO.declarer)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
				if (p == 1-SUARO.declarer)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "defender\n");
			}
			if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
					char bid_text[game.max_bid_length];
					game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
					if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
				};
	send_player_message_toall(s);
}

static void suaro_end_trick()
{
	player_t hi_player = game.leader, p, lo_player = game.leader;
	card_t hi_card = game.lead_card, lo_card = game.lead_card;

	/* default method of winning tricks: the winning card is the highest
	 * card of the suit lead, or the highest trump if there is trump */
	for (p=0; p<game.num_players; p++) {
		card_t card = game.seats[ game.players[p].seat ].table;
		card = game.funcs->map_card( card );
		if ( (card.suit == game.trump && (hi_card.suit != game.trump || hi_card.face < card.face))
		   || (card.suit == hi_card.suit && card.face > hi_card.face) ) {
			hi_card = card;
			hi_player = p;
		}
		if (card.suit == lo_card.suit && card.face < lo_card.face) {
			/* tracking the low card is unnecessary in most cases,
			 * but it saves us trouble in a few cases (like "low" bids in Suaro) */
			lo_card = card;
			lo_player = p;
		}
	}

			if (SUARO.contract_suit == SUARO_LOW) {
				/* low card wins */
				hi_player = lo_player;
				hi_card = lo_card;
			}




	if (game.last_trick) {
		/* this sets up a "last trick" message */
		int p_r;
		char message[512];
		int msg_len = 0;
		card_t card;
		for (p_r=0; p_r<game.num_players; p_r++) {
			p = (game.leader + p_r) % game.num_players;
			card = game.seats[ game.players[p].seat ].table;
			msg_len += snprintf(message+msg_len, sizeof(message)-msg_len,
					"%s - %s of %s\n",
					ggz_seats[p].name,
					face_names[(int)card.face],
					suit_names[(int)card.suit]
					/*, p == hi_player ? " (winner)" : "" */
					);
		}
		set_global_message("Last Trick", "%s", message);
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	game_set_player_message(hi_player);		

}






