/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ La Pocha game module
 * Date: 06/29/2000
 * Desc: Game functions
 * $Id: game.c 4026 2002-04-20 21:57:36Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <unistd.h>
#include <ggz.h>

#include "cards.h"
#include "game.h"

/* Global game variables */
struct lp_game_t game;

/* Private functions */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_bid(char bid);
static int game_send_play(char card);
static int game_send_sync(int num);
static int game_send_gameover(void);
static void game_play(void);
static int game_req_bid(void);
static int game_req_play(void);
static int game_req_trump(void);
static int game_handle_bid(int num, char *bid);
static int game_handle_play(int num, char *card);
static int game_update(int event, void *data);
static int game_generate_next_hand(void);
static int game_send_hand(int);
static int game_send_trump(int);
static int game_receive_trump(int);
static void game_score_trick(void);
static void game_score_hand(void);


/* Setup game state and board */
void game_init(GGZdMod *ggz)
{
	int i;
	
	game.ggz = ggz;
	
	game.turn = -1;
	game.state = LP_STATE_INIT;
	for(i=0; i<4; i++)
		game.score[0] = 0;
}


/* Handle message from GGZ server */
void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, void* data)
{
	GGZdModState old_state = *(GGZdModState*)data;
	if (old_state == GGZDMOD_STATE_CREATED)
		game_update(LP_EVENT_LAUNCH, NULL);
}


void game_handle_ggz_join(GGZdMod *ggz, GGZdModEvent event, void* data)
{
	int player = ((GGZSeat*)data)->num;
	game_update(LP_EVENT_JOIN, &player);
}

void game_handle_ggz_leave(GGZdMod *ggz, GGZdModEvent event, void* data)
{
	int player = ((GGZSeat*)data)->num;
	game_update(LP_EVENT_LEAVE, &player);
}


/* Handle message from player */
void game_handle_player(GGZdMod *ggz, GGZdModEvent event, void* data)
{
	int num = *(int*)data;
	int fd, op;
	char bid, card;

	fd = ggzdmod_get_seat(game.ggz, num).fd;
	
	if(fd < 0 || ggz_read_int(fd, &op) < 0)
		return;

	switch(op) {
		case LP_SND_BID:
			if(game_handle_bid(num, &bid) == 0)
				game_update(LP_EVENT_BID, &bid);
			break;
		case LP_SND_PLAY:
			if(game_handle_play(num, &card) == 0)
				game_update(LP_EVENT_PLAY, &card);
			break;
		case LP_REQ_SYNC:
			game_send_sync(num);
			break;
		case LP_SND_TRUMP:
			if(game_receive_trump(num) == 0)
				game_update(LP_EVENT_TRUMP, NULL);
			break;
		/*case LP_REQ_NEWGAME:
			if(game.state == LP_STATE_DONE)
				game_start_newgame();
			status = 0;
			break;*/
		default:
			/* Unrecognized opcode */
			ggzdmod_log(game.ggz, "ERROR: unrecognized player opcode %d.", op);
			break;
	}
}


/* Send out seat assignment */
static int game_send_seat(int seat)
{
	int fd = ggzdmod_get_seat(game.ggz, seat).fd;

	ggzdmod_log(game.ggz, "Sending player %d's seat num", seat);

	if(ggz_write_int(fd, LP_MSG_SEAT) < 0
	   || ggz_write_int(fd, seat) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
static int game_send_players(void)
{
	int i, j, fd;
	
	for(j=0; j<4; j++) {
		if((fd = ggzdmod_get_seat(game.ggz, j).fd) == -1)
			continue;

		ggzdmod_log(game.ggz, "Sending player list to player %d", j);

		if(ggz_write_int(fd, LP_MSG_PLAYERS) < 0)
			return -1;
	
		for(i=0; i<4; i++) {
			GGZSeat seat = ggzdmod_get_seat(game.ggz, i);
			if(ggz_write_int(fd, seat.type) < 0)
				return -1;
			if(seat.type != GGZ_SEAT_OPEN
			    && ggz_write_string(fd, seat.name) < 0)
				return -1;
		}
	}
	return 0;
}


/* Send out bid for player */
static int game_send_bid(char bid)
{
	int i;
	int fd;

	for(i=0; i<4; i++) {
		fd = ggzdmod_get_seat(game.ggz, i).fd;
		/* If self or a computer, don't need to send */
		if(i == game.bid_now || fd == -1)
			continue;
		ggzdmod_log(game.ggz, "Sending player %d's bid to player %d",
			   game.bid_now, i);
		if(ggz_write_int(fd, LP_MSG_BID) < 0
		   || ggz_write_char(fd, game.bid_now) < 0
		   || ggz_write_char(fd, bid) < 0)
			return -1;
	}
	
	return 0;
}


/* Send out play for player */
static int game_send_play(char card)
{
	int i;
	int fd;

	for(i=0; i<4; i++) {
		fd = ggzdmod_get_seat(game.ggz, i).fd;
		/* If self or a computer, don't need to send */
		if(i == game.turn || fd == -1)
			continue;
		ggzdmod_log(game.ggz, "Sending player %d's play to player %d",
			   game.turn, i);
		if(ggz_write_int(fd, LP_MSG_PLAY) < 0
		   || ggz_write_char(fd, game.turn) < 0
		   || ggz_write_char(fd, card) < 0)
			return -1;
	}
	
	return 0;
}


/* Send out current game hand, score, etc */
static int game_send_sync(int num)
{	
	int i, fd = ggzdmod_get_seat(game.ggz, num).fd;

	ggzdmod_log(game.ggz, "Handling sync for player %d", num);

	if(ggz_write_int(fd, LP_SND_SYNC) < 0
	   || ggz_write_char(fd, game.turn) < 0)
		return -1;
	
	/* First send scores */
	for(i=0; i<4; i++)
		if(ggz_write_int(fd, game.score[i]) < 0)
			return -1;

	/* Send out hand */
	if(ggz_write_char(fd, game.hand[num].hand_size) < 0)
		return -1;
	for(i=0; i<game.hand[num].hand_size; i++)
		if(ggz_write_char(fd, game.hand[num].card[i]) < 0)
			return -1;


	/* Send out the game state */
	if(ggz_write_char(fd, game.saved_state) < 0)
		return -1;

	/* Send out info necessary for current state */
	switch(game.saved_state) {
		case LP_STATE_INIT:
			/* Can't occur */
			break;
		case LP_STATE_WAIT:
			/* Can't occur */
			break;
		case LP_STATE_NEW_HAND:
			/* Can't occur */
			break;
		case LP_STATE_GET_TRUMP:
			/* Notify of dealer */
			if(ggz_write_char(fd, game.dealer) < 0)
				return -1;

			break;
		case LP_STATE_BIDDING:
			/* Notify of dealer and trump */
			if(ggz_write_char(fd, game.dealer) < 0
			   || ggz_write_char(fd, game.trump) < 0)
				return -1;

			/* Send all four bids */
			for(i=0; i<4; i++)
				if(ggz_write_int(fd, game.bid[i]) < 0)
					return -1;

			break;
		case LP_STATE_PLAYING:
			/* Notify of dealer, leader, trump and led suit */
			if(ggz_write_char(fd, game.dealer) < 0
			   || ggz_write_char(fd, game.leader) < 0
			   || ggz_write_char(fd, game.trump) < 0
			   || ggz_write_char(fd, game.led_suit) < 0)
				return -1;

			/* Send all four bids, trick counts and cards on table*/
			for(i=0; i<4; i++)
				if(ggz_write_int(fd, game.bid[i]) < 0)
					return -1;
			for(i=0; i<4; i++)
				if(ggz_write_int(fd, game.tricks[i]) < 0)
					return -1;
			for(i=0; i<4; i++)
				if(ggz_write_char(fd, game.table[i]) < 0)
					return -1;

			break;
		case LP_STATE_DONE:
			/* Can't occur */
			break;
	}

	return 0;
}
	

/* Send out game-over message */
static int game_send_gameover(void)
{
	int i, fd, hi_score;
	int status = 0;
	char winner = -1;

	hi_score = -9999;
	for(i=0; i<4; i++) {
		if(game.score[i] == 0) {
			winner = i;
			hi_score = 9999;
		} else if(game.score[i] > hi_score) {
			winner = i;
			hi_score = game.score[i];
		}
	}

	for(i=0; i<4; i++) {
		if((fd = ggzdmod_get_seat(game.ggz, i).fd) == -1)
			continue;

		ggzdmod_log(game.ggz, "Sending game-over to player %d", i);

		if(ggz_write_int(fd, LP_MSG_GAMEOVER) < 0
		    || ggz_write_char(fd, winner) < 0)
			status = -1;
	}

	return status;
}


/* Do the next play */
static void game_play(void)
{
	int i;

	switch(game.state) {
		case LP_STATE_NEW_HAND:
			if(game_generate_next_hand() < 0)
				return;
			if(game.state == LP_STATE_DONE) {
				game_send_gameover();
				return;
			}
			game.bid_now = (game.dealer + 1) % 4;
			game.bid_count = 0;
			game.bid_total = 0;
			for(i=0; i<4; i++) {
				game.bid[i] = -1;
				game.tricks[i] = 0;
				game.table[i] = -1;
			}
			if(game.state == LP_STATE_BIDDING)
				game_req_bid();
			break;
		case LP_STATE_GET_TRUMP:
			game_req_trump();
			break;
		case LP_STATE_BIDDING:
			if(game.bid_count < 4)
				game_req_bid();
			else {
				game.turn = (game.dealer + 1) % 4;
				game.leader = game.turn;
				game.tricks_left = game.hand[0].hand_size;
				for(i=0; i<4; i++)
					game.table[i] = -1;
				game.state = LP_STATE_PLAYING;
				game.play_count = 0;
				game_req_play();
			}
			break;
		case LP_STATE_PLAYING:
			if(game.play_count < 4)
				game_req_play();
			else {
				sleep(1);
				game_score_trick();
				if(game.tricks_left == 0) {
					sleep(1);
					game_score_hand();
					game_play();	/* A minor recursion */
						/* as state will = NEW_HAND  */
				} else {
					game.play_count = 0;
					game_req_play();
				}
			}
			break;
	}

/* THIS STUFF WILL MOVE TO game_req_* functions
 *
 *	if(ggzd_get_seat_status(num) == GGZ_SEAT_BOT) {
 *		card = ai_play_card();
 *		game_update(LP_EVENT_PLAY, &card);
 *	} else
 */

	return;
}


/* Request bid from current bidder */
static int game_req_bid(void)
{
	int fd = ggzdmod_get_seat(game.ggz, game.bid_now).fd;

	if(ggz_write_int(fd, LP_REQ_BID) < 0)
		return -1;

	return 0;
}


/* Request play from current player */
static int game_req_play()
{
	int fd = ggzdmod_get_seat(game.ggz, game.turn).fd;

	if(ggz_write_int(fd, LP_REQ_PLAY) < 0)
		return -1;

	return 0;
}


/* Request trump from the dealer */
static int game_req_trump()
{
	int fd = ggzdmod_get_seat(game.ggz, game.dealer).fd;

	if(ggz_write_int(fd, LP_REQ_TRUMP) < 0)
		return -1;

	return 0;
}


/* Handle incoming bid from player */
static int game_handle_bid(int num, char *bid)
{
	int fd = ggzdmod_get_seat(game.ggz, num).fd;
	char status = 0;

	ggzdmod_log(game.ggz, "Handling bid from player %d", num);
	if(ggz_read_char(fd, bid) < 0)
		return -1;

	/* First of all, is it their turn to bid? */
	if(game.bid_now != num || game.state != LP_STATE_BIDDING)
		status = LP_ERR_TURN;
	/* Sane bid? */
	else if(*bid > game.hand[0].hand_size || *bid < 0)
		status = LP_ERR_INVALID;
	/* Finally, special restriction for dealer bid */
	else if(game.bid_now == game.dealer) {
		/* Dealer cannot make total bid = number of tricks */
		if(game.bid_total + *bid == game.hand[0].hand_size)
			status = LP_ERR_INVALID;
	}

	/* Send status to bidder */
	if(ggz_write_int(fd, LP_RSP_BID) < 0
	   || ggz_write_char(fd, status) < 0)
		return -1;

	if(status == 0)
		game.bid[num] = *bid;

	return (int)status;
}


/* Handle incoming play from player */
static int game_handle_play(int num, char *card)
{
	int fd = ggzdmod_get_seat(game.ggz, num).fd;
	char status;
	char card_num, c;
	char hi_trump, hi_trump_played;
	int i;
	
	ggzdmod_log(game.ggz, "Handling play for player %d", num);
	if(ggz_read_char(fd, &card_num) < 0)
		return -1;

	/* Look up the card's value */
	*card = game.hand[num].card[(int)card_num];

	/* Check for a valid play */
	status = 0;
	if(game.turn != num)
		status = LP_ERR_TURN;
	else if(*card == -1)
		status = LP_ERR_INVALID;
	else if(game.turn != game.leader) {
		/* This is not the leader, so validate */
		if((*card/13) != game.led_suit) {
			/* Player didn't follow suit ... */
			if(cards_suit_in_hand(&game.hand[num], game.led_suit))
				/* Player could have followed suit */
				status = LP_ERR_FOLLOW_SUIT;
			else if((*card/13) != game.trump) {
				/* This is where things get weird, the */
				/* player doesn't have led suit, and   */
				/* he didn't trump.  But he MUST trump */
				/* IF he has a trump card higher than  */
				/* any trump card on the table.  YEEK! */
				hi_trump= cards_highest_in_suit(&game.hand[num],
								 game.trump);
				hi_trump_played = 0;
				for(i=0; i<4; i++) {
					c = game.table[i];
					if(c != -1
					   && (c / 13) == game.trump
					   && card_value(c) > hi_trump_played)
						hi_trump_played = card_value(c);
				}
				if(hi_trump > hi_trump_played)
					status = LP_ERR_MUST_TRUMP;
			}
		}
	}

	/* Send back play status */
	if(ggz_write_int(fd, LP_RSP_PLAY) < 0
	    || ggz_write_char(fd, status))
		return -1;

	/* Move the card onto the table */
	if(status == 0) {
		game.hand[num].card[(int)card_num] = -1;
		game.table[num] = *card;
		if(num == game.leader)
			game.led_suit = *card/13;
	}

	return status;
}


/* Get the trump suit from the dealer */
static int game_receive_trump(int num)
{
	int fd = ggzdmod_get_seat(game.ggz, num).fd;
	char status;

	/* If it ain't the dealer, just ignore */
	if(num != game.dealer)
		return -1;

	if(ggz_read_char(fd, &game.trump) < 0)
		return -1;

	if(game.trump >= 0 && game.trump < 4)
		status = 0;
	else
		status = LP_ERR_INVALID;

	if(ggz_write_int(fd, LP_RSP_TRUMP) < 0
	   || ggz_write_char(fd, status) < 0)
		return -1;

	return (int)status;
}


static int seats_full(void)
{
	return ggzdmod_count_seats(game.ggz, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(game.ggz, GGZ_SEAT_RESERVED) == 0;
}


/* Update game state */
static int game_update(int event, void *data)
{
	int seat, i;
	char card, bid;
	
	switch(event) {
		case LP_EVENT_LAUNCH:
			if(game.state != LP_STATE_INIT)
				return -1;
			game.state = LP_STATE_WAIT;
			break;
		case LP_EVENT_JOIN:
			if(game.state != LP_STATE_WAIT)
				return -1;

			/* Send out seat assignments and player list */
			seat = *(int*)data;
			game_send_seat(seat);
			game_send_players();

			/* Rejoiner must learn table info */
			if(game.turn != -1)
				game_send_sync(seat);

			/* All the seats are occupied ? */
			if(seats_full()) {
				/* If first time, then initialize */
				if(game.turn == -1) {
					game.turn = 0;
					game.dealer = 0;
					game.hand_num = 1;
					game.state = LP_STATE_NEW_HAND;
				} else
					/* Restore state */
					game.state = game.saved_state;

				/* (Re)Start game play */
				game_play();
			}
			break;
		case LP_EVENT_LEAVE:
			game_send_players();
			if(game.state != LP_STATE_WAIT)
				game.saved_state = game.state;
			game.state = LP_STATE_WAIT;
			break;
		case LP_EVENT_PLAY:
			card = *(char*)data;
			game_send_play(card);

			/* Request next move */
			game.turn = (game.turn + 1) % 4;
			game.play_count++;
			game_play();
			break;
		case LP_EVENT_BID:
			bid = *(char*)data;
			game_send_bid(bid);
			if(++game.bid_count < 4) {
				game.bid_total += bid;
				game.bid_now = (game.bid_now + 1) % 4;
			}

			/* Request next move */
			game_play();
			break;
		case LP_EVENT_TRUMP:
			for(i=0; i<4; i++)
				game_send_trump(i);
			game.state = LP_STATE_BIDDING;

			game_play();
			break;
	}
	
	return 0;
}


/* This seemed easier than writing an algorithm to compute the */
/* number of cards dealt for each hand */
static int card_count[] = { 1, 1, 1, 1,
			    2, 3, 4, 5, 6, 7, 8, 9,
			    10, 10, 10, 10,
			    9, 8, 7, 6, 5, 4, 3, 2,
			    1, 1, 1, 1 };

/* Generate and send out a new hand */
static int game_generate_next_hand(void)
{
	int p, result=0;

	/* Test for end of game */
	if(game.hand_num == 29) {
		game.state = LP_STATE_DONE;
		return 0;
	}

	/* First, shuffle and deal a hand */
	cards_shuffle_deck(GGZ_DECK_LAPOCHA);
	cards_deal_hands(4, card_count[game.hand_num-1], game.hand);

	/* Now send the resulting hands to each player */
	for(p=0; p<4; p++) {
		/* Note any errors, but keep sending cards */
		/* to players who are still connected */
		if(game_send_hand(p) < 0)
			result = -1;
	}

	/* Select our trump, use a card cut if hand size != 10 */
	if(card_count[game.hand_num-1] != 10) {
		game.state = LP_STATE_BIDDING;
		game.trump = cards_cut_for_trump();
		for(p=0; p<4; p++) {
			if(game_send_trump(p) < 0)
				result = -1;
		}
	} else {
		game.state = LP_STATE_GET_TRUMP;
		game_req_trump();
	}

	return result;
}


/* Send a player their hand */
static int game_send_hand(int seat)
{
	int fd = ggzdmod_get_seat(game.ggz, seat).fd;
	int i;

	/* Don't send to a bot */
	if(fd == -1)
		return 0;

	ggzdmod_log(game.ggz, "Sending player %d their hand", seat);

	if(ggz_write_int(fd, LP_MSG_HAND) < 0
	   || ggz_write_char(fd, game.dealer) < 0
	   || ggz_write_char(fd, game.hand[seat].hand_size) < 0)
		return -1;

	for(i=0; i<game.hand[seat].hand_size; i++)
		if(ggz_write_char(fd, game.hand[seat].card[i]) < 0)
			return -1;

	return 0;
}


/* Send trump to a player */
static int game_send_trump(int seat)
{
	int fd = ggzdmod_get_seat(game.ggz, seat).fd;

	/* Don't send to a bot */
	if(fd == -1)
		return 0;

	ggzdmod_log(game.ggz, "Sending trump suit to player %d", seat);

	if(ggz_write_int(fd, LP_MSG_TRUMP) < 0
	   || ggz_write_char(fd, game.trump) < 0)
		return -1;

	return 0;
}


/* Figure who won the trick and announce */
static void game_score_trick(void)
{
	int i, hi_card, hi_player, trumped;
	int fd;

	hi_card = hi_player = -1;
	if(game.led_suit == game.trump)
		trumped = 1;
	else
		trumped = 0;
	for(i=0; i<4; i++) {
		if(!trumped
		   && game.table[i]/13 == game.led_suit
		   && card_value(game.table[i]) > hi_card) {
			hi_card = card_value(game.table[i]);
			hi_player = i;
		} else if(!trumped && game.table[i]/13 == game.trump) {
			hi_card = card_value(game.table[i]);
			hi_player = i;
			trumped++;
		} else if(game.table[i]/13 == game.trump
			  && card_value(game.table[i]) > hi_card) {
				hi_card = card_value(game.table[i]);
				hi_player = i;
		}
	}

	game.tricks[hi_player]++;
	game.turn = game.leader = hi_player;
	game.tricks_left--;

	for(i=0; i<4; i++) {
		game.table[i] = -1;

		if((fd = ggzdmod_get_seat(game.ggz, i).fd) == -1)
			continue;

		ggzdmod_log(game.ggz, "Sending trick result to player %d", i);

		if(ggz_write_int(fd, LP_MSG_TRICK) == 0)
			ggz_write_char(fd, (char)hi_player);
	}
}


/* Calculate scores for this hand and announce */
void game_score_hand(void)
{
	int i, fd;

	for(i=0; i<4; i++) {
		ggzdmod_log(game.ggz, "Player %d got %d tricks on a bid of %d", i,
			  game.tricks[i], game.bid[i]);
		if(game.tricks[i] == game.bid[i])
			game.score[i] += 10 + (5 * game.bid[i]);
		else
			game.score[i] -= 5 * game.bid[i];
	}

	game.dealer = (4 + game.dealer - 1) % 4;
	game.hand_num++;
	game.state = LP_STATE_NEW_HAND;

	for(i=0; i<4; i++) {
		if((fd = ggzdmod_get_seat(game.ggz, i).fd) == -1)
			continue;

		ggzdmod_log(game.ggz, "Sending scores to player %d", i);

		if(ggz_write_int(fd, LP_MSG_SCORES) < 0
		   || ggz_write_int(fd, game.score[0]) < 0
		   || ggz_write_int(fd, game.score[1]) < 0
		   || ggz_write_int(fd, game.score[2]) < 0
		   || ggz_write_int(fd, game.score[3]) < 0)
			;

		/* Brush their card from teh table */
		game.table[i] = -1;
	}
}
