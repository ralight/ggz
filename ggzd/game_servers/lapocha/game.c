/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ La Pocha game module
 * Date: 06/29/2000
 * Desc: Game functions
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

#include <stdlib.h>

#include <easysock.h>

#include "ggz.h"
#include "cards.h"
#include "game.h"
#include "protocols.h"

/* Global game variables */
struct lp_game_t game;

/* Private functions */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_bid(char bid);
static int game_send_play(char card);
static int game_send_sync(int num);
static int game_send_gameover(char winner);
static void game_play(void);
static int game_req_bid(void);
static int game_req_play(void);
static int game_handle_bid(int num, char *bid);
static int game_handle_play(int num, char *card);
static int game_update(int event, void *data);
static int game_generate_next_hand(void);
static int game_send_hand(int);
static int game_send_trump(int);


/* Setup game state and board */
void game_init(void)
{
	int i;
	
	game.turn = -1;
	game.state = LP_STATE_INIT;
	for(i=0; i<4; i++)
		game.score[0] = 0;
}


/* Handle message from GGZ server */
int game_handle_ggz(int ggz_fd, int* p_fd)
{
	int op, seat, status = -1;

	if(es_read_int(ggz_fd, &op) < 0)
		return -1;

	switch(op) {
		case REQ_GAME_LAUNCH:
			if(ggz_game_launch() == 0)
				status = game_update(LP_EVENT_LAUNCH, NULL);
			break;
		case REQ_GAME_JOIN:
			if(ggz_player_join(&seat, p_fd) == 0) {
				status = game_update(LP_EVENT_JOIN, &seat);
				status = 1;
			}
			break;
		case REQ_GAME_LEAVE:
			if((status = ggz_player_leave(&seat, p_fd)) == 0) {
				game_update(LP_EVENT_LEAVE, &seat);
				status = 2;
			}
			break;
		case RSP_GAME_OVER:
			status = 3; /* Signal safe to exit */
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}
	return status;
}


/* Handle message from player */
int game_handle_player(int num)
{
	int fd, op, status;
	char bid, card;

	fd = ggz_seats[num].fd;
	
	if(es_read_int(fd, &op) < 0)
		return -1;

	switch(op) {
		case LP_SND_BID:
			if((status = game_handle_bid(num, &bid)) == 0)
				game_update(LP_EVENT_BID, &bid);
			break;
		case LP_SND_PLAY:
			if((status = game_handle_play(num, &card)) == 0)
				game_update(LP_EVENT_PLAY, &card);
			break;
		case LP_REQ_SYNC:
			status = game_send_sync(num);
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}
	return status;
}


/* Send out seat assignment */
static int game_send_seat(int seat)
{
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d's seat num", seat);

	if(es_write_int(fd, LP_MSG_SEAT) < 0
	   || es_write_int(fd, seat) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
static int game_send_players(void)
{
	int i, j, fd;
	
	for(j=0; j<4; j++) {
		if((fd = ggz_seats[j].fd) == -1)
			continue;

		ggz_debug("Sending player list to player %d", j);

		if(es_write_int(fd, LP_MSG_PLAYERS) < 0)
			return -1;
	
		for(i=0; i<4; i++) {
			if(es_write_int(fd, ggz_seats[i].assign) < 0)
				return -1;
			if(ggz_seats[i].assign != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggz_seats[i].name) < 0) 
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
		fd = ggz_seats[i].fd;
		/* If self or a computer, don't need to send */
		if(i == game.bid_now || fd == -1)
			continue;
		ggz_debug("Sending player %d's bid to player %d",
			   game.bid_now, i);
		if(es_write_int(fd, LP_MSG_BID) < 0
		   || es_write_char(fd, game.bid_now) < 0
		   || es_write_char(fd, bid) < 0)
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
		fd = ggz_seats[i].fd;
		/* If self or a computer, don't need to send */
		if(i == game.turn || fd == -1)
			continue;
		ggz_debug("Sending player %d's play to player %d",
			   game.turn, i);
		if(es_write_int(fd, LP_MSG_PLAY) < 0
		   || es_write_char(fd, game.turn) < 0
	/* TODO: Send card value, not number */
		   || es_write_char(fd, card) < 0)
			return -1;
	}
	
	return 0;
}


/* Send out current game hand, score, etc */
static int game_send_sync(int num)
{	
	int i, fd = ggz_seats[num].fd;

	ggz_debug("Handling sync for player %d", num);

	if(es_write_int(fd, LP_SND_SYNC) < 0
	   || es_write_char(fd, game.turn) < 0)
		return -1;
	
	for(i=0; i<4; i++)
		if(es_write_int(fd, game.score[0]) < 0)
			return -1;

	/* TODO: Send out hand */

	return 0;
}
	

/* Send out game-over message */
static int game_send_gameover(char winner)
{
	int i, fd;
	
	for(i=0; i<4; i++) {
		if((fd = ggz_seats[i].fd) == -1)
			continue;

		ggz_debug("Sending game-over to player %d", i);

		if(es_write_int(fd, LP_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}
	return 0;
}


/* Do the next play */
static void game_play(void)
{
	int card;

	switch(game.state) {
		case LP_STATE_NEW_HAND:
			if(game_generate_next_hand() < 0)
				return;
			game.state = LP_STATE_BIDDING;
			game.bid_now = (game.dealer + 1) % 4;
			game.bid_count = 0;
			game_req_bid();
			break;
		case LP_STATE_BIDDING:
			if(game.bid_count < 4)
				game_req_bid();
			else {
				game.turn = (game.dealer + 1) % 4;
				game.state = LP_STATE_PLAYING;
				game_req_play();
			}
			break;
	}

/* THIS STUFF WILL MOVE TO game_req_* functions
 *
 *	if(ggz_seats[num].assign == GGZ_SEAT_BOT) {
 *		card = ai_play_card();
 *		game_update(LP_EVENT_PLAY, &card);
 *	} else
 */

	return;
}


/* Request bid from current bidder */
static int game_req_bid(void)
{
	int fd = ggz_seats[game.bid_now].fd;

	if(es_write_int(fd, LP_REQ_BID) < 0)
		return -1;

	return 0;
}


/* Request play from current player */
static int game_req_play()
{
	int fd = ggz_seats[game.turn].fd;

	if(es_write_int(fd, LP_REQ_PLAY) < 0)
		return -1;

	return 0;
}


/* Handle incoming bid from player */
static int game_handle_bid(int num, char *bid)
{
	int fd = ggz_seats[num].fd;
	char status = 0;
	int total_bid;

	ggz_debug("Handling bid from player %d", num);
	if(es_read_char(fd, bid) < 0)
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
	if(es_write_int(fd, LP_RSP_BID) < 0
	   || es_write_char(fd, status) < 0)
		return -1;

	return (int)status;
}


/* Handle incoming play from player */
static int game_handle_play(int num, char *card)
{
	int fd = ggz_seats[num].fd;
	char status;
	
	ggz_debug("Handling play for player %d", num);
	if(es_read_char(fd, card) < 0)
		return -1;

	/* TODO: Valid play? */
	status = 0;

	/* Send back play status */
	if(es_write_int(fd, LP_RSP_PLAY) < 0
	    || es_write_char(fd, status))
		return -1;

	/* If move simply invalid, ask for resubmit */
	if((status == LP_ERR_INVALID)
	     && game_req_play() < 0)
		return -1;

	if(status < 0)
		return 1;
	
	game.turn = (game.turn + 1) % 4;

	return 0;
}


/* Update game state */
static int game_update(int event, void *data)
{
	int seat;
	char card;
	char bid;
	
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

			if(!ggz_seats_open()) {
				if(game.turn == -1) {
					game.turn = 0;
					game.dealer = 0;
					game.hand_num = 1;
					game.state = LP_STATE_NEW_HAND;
				} else {
					game_send_sync(seat);
					game.state = LP_STATE_PLAYING;
				}
			
				game_play();
			}
			break;
		case LP_EVENT_LEAVE:
			game_send_players();
			if(game.state == LP_STATE_PLAYING)
				game.state = LP_STATE_WAIT;
			break;
		case LP_EVENT_PLAY:
			card = *(char*)data;
			game_send_play(card);
		
			/* TODO: End of hand?  End of game? */

			/* Request next move */
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
		game.trump = cards_cut_for_trump();
		for(p=0; p<4; p++) {
			if(game_send_trump(p) < 0)
				result = -1;
		}
	} /* else LP_REQ_TRUMP */

	return result;
}


/* Send a player their hand */
static int game_send_hand(int seat)
{
	int fd = ggz_seats[seat].fd;
	int i;

	/* Don't send to a bot */
	if(fd == -1)
		return 0;

	ggz_debug("Sending player %d their hand", seat);

	if(es_write_int(fd, LP_MSG_HAND) < 0
	   || es_write_char(fd, game.hand[seat].hand_size) < 0)
		return -1;

	for(i=0; i<game.hand[seat].hand_size; i++)
		if(es_write_char(fd, game.hand[seat].card[i]) < 0)
			return -1;

	return 0;
}


/* Send trump to all players */
static int game_send_trump(int seat)
{
	int fd = ggz_seats[seat].fd;

	/* Don't send to a bot */
	if(fd == -1)
		return 0;

	ggz_debug("Sending trump suit to player %d", seat);

	if(es_write_int(fd, LP_MSG_TRUMP) < 0
	   || es_write_char(fd, game.trump) < 0)
		return -1;
}
