/* 
 * File: games/spades.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game functions for Spades
 * $Id: spades.c 8911 2007-01-15 03:18:12Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <stdio.h>
#include <string.h>

#include "net_common.h"
#include "protocol.h"

#include "bid.h"
#include "common.h"
#include "game.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "score.h"
#include "team.h"

#include "spades.h"

/* Nil tricks options: different ways of scoring tricks taken by nil/dnil
   bidders.

   NIL_TRICKS_COUNT - The tricks count toward the partner's contract
   directly.

   NIL_TRICKS_DONT_COUNT - The tricks do not count toward the partner's
   contract, but still count as bags and give overtrick points.

   NIL_TRICKS_NO_POINT - The tricks do not count toward partner's contract,
   and give no overtrick points, but still count as bags.  (This bizarre
   option is included for compatibility with other gaming zones.)
*/
enum nil_option {
	NIL_TRICKS_COUNT,
	NIL_TRICKS_DONT_COUNT,
	NIL_TRICKS_NO_POINTS,
	NIL_TRICKS_LAST
};

enum handlimit_option {
	HANDS_UNLIMITED,
	HANDS_ONE,
	HANDS_THREE,
	HANDS_SIX,
	HANDS_EIGHT,
	HANDS_TEN
};

enum bid_variant {
	/* Regular bidding (i.e., spades) */
	BID_NORMAL,

	/* In "whiz" everybody has to bid ne number of spades they are
	   holding (or nil/dn) */
	BID_WHIZ,

	/* In the DN variant everybody always has to bid dnil */
	BID_DN_ONLY,
};

#define GSPADES ( *(spades_game_t *)(game.specific) )
typedef struct spades_game_t {
	/* options */
	int nil_value;		/* 0 for none; generally 50 or 100 */
	int double_nil_value;	/* 0 for none; generally 100 or 200 */
	int minimum_team_bid;	/* the minimum bid by one team */
	enum nil_option nil_tricks_count;	/* See enum explanation */
	bool unmakeable_bids;	/* Whether team bids of over 13 are allowed. */
	bool zero_bids;		/* Whether bids of 0 are allowed. */
	enum bid_variant bid_variant;	/* See enum explanation */
	bool variant301;	/* 301 - highest score below 301 wins */

	/* data */
	int show_hand[4];	/* this is 0 if we're supposed to conceal the
				   hand (for blind bids */
} spades_game_t;

#define BAGS(team) (game.teams[(team)].score.extra[0])

static bool spades_is_valid_game(void);
static void spades_init_game(void);
static void spades_get_options(void);
static int spades_handle_option(char *option, int value);
static char *spades_get_option_text(char *buf, int bufsz, char *option,
				    int value);
static void spades_start_bidding(void);
static void spades_get_bid(void);
static void spades_handle_bid(player_t p, bid_t bid);
static void spades_next_bid(void);
static void spades_get_play(player_t p);
static int spades_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static int spades_get_bid_desc(char *buf, size_t buf_len, bid_t bid);
static void spades_set_player_message(player_t p);
static void spades_sync_player(player_t p);
static void spades_deal_hand(void);
static void spades_end_hand(void);
static void spades_start_game(void);
static void spades_handle_gameover(void);
static void spades_send_hand(player_t p, seat_t s);

game_data_t spades_data = {
	"spades",
	N_("Spades"),
	"http://pagat.com/boston/spades.html",
	spades_is_valid_game,
	spades_init_game,
	spades_get_options,
	spades_handle_option,
	spades_get_option_text,
	spades_set_player_message,
	spades_sync_player,
	spades_get_bid_text,
	spades_get_bid_desc,
	spades_start_bidding,
	spades_get_bid,
	spades_handle_bid,
	spades_next_bid,
	game_start_playing,
	game_verify_play,
	game_next_play,
	spades_get_play,
	game_handle_play,
	spades_deal_hand,
	game_end_trick,
	spades_end_hand,
	spades_start_game,
	game_test_for_gameover,
	spades_handle_gameover,
	game_map_card,
	game_compare_cards,
	spades_send_hand
};

static void spd_send_scoredata(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	team_t team;
	int waiting = -1;

	if (!dio) {
		/* This can happen, for instance with empty spectator seats. */
		return;
	}

	/* The score data can be used by the AI for
	   calculations or by the game client to
	   display team score/bid/tricks/bags. */
	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_SPECIFIC);

	for (team = 0; team < 2; team++) {
		ggz_dio_put_int(dio, get_team_score(team));
		ggz_dio_put_int(dio, BAGS(team));
	}
	players_iterate(p2) {
		ggz_dio_put_int(dio, game.players[p2].tricks);
	}
	players_iterate_end;
	ggz_dio_put_int(dio, game.next_bid);
	ggz_dio_put_int(dio, game.next_play);
	if (game.state == STATE_WAIT_FOR_BID) {
		waiting = game.next_bid;
	} else if (game.state == STATE_WAIT_FOR_PLAY) {
		waiting = game.next_play;
	}
	ggz_dio_put_int(dio, waiting);	/* The player we're waiting on. */
	ggz_dio_packet_end(dio);
}

static void spd_broadcast_scoredata(void)
{
	allplayers_iterate(p) {
		spd_send_scoredata(p);
	} allplayers_iterate_end;
}

static bool spades_is_valid_game(void)
{
	return (game.num_players == 4);
}

static void spades_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(spades_game_t));

	set_num_seats(4);
	set_num_teams(2);
	for (s = 0; s < game.num_players; s++) {
		assign_seat(s, s);	/* one player per seat */
		assign_team(s % 2, s);
	}

	game.must_break_trump = 1;	/* in spades, you can't lead trump
					   until it's broken */
	game.target_score = 500;	/* adjustable by options */
	game.num_extra_scores = 1;	/* Bags */
	set_trump_suit(SPADES);

	GSPADES.nil_value = 100;
	GSPADES.nil_tricks_count = NIL_TRICKS_COUNT;
	GSPADES.unmakeable_bids = FALSE;
	GSPADES.zero_bids = TRUE;
	GSPADES.bid_variant = BID_NORMAL;
	GSPADES.variant301 = FALSE;
}

static void spades_get_options(void)
{
	/* three options: target score: 100, 250, 500, 1000 nil value: 0, 50, 
	   100 minimum team bid: 0, 1, 2, 3, 4 double nil value: 0, 100, 200 */
	add_option("Nil", "nil_value",
		   "How much is nil worth (win or lose)?",
		   3, 2,
		   "No nil bids", "Nil is worth 50", "Nil is worth 100");
	add_option("Nil", "double_nil",
		   "How many points is blind (double) nil worth (win or lose)?",
		   3, 0,
		   "No blind (double) nil",
		   "Blind (double) nil worth 100",
		   "Blind (double) nil worth 200");
	add_option("Gameover", "max_hands",
		   "What is the maximum number of hands that will be played?",
		   6, 0,
		   "No maximum hand limit",
		   "Just play one hand",
		   "Max of 3 hands",
		   "Max of 6 hands", "Max of 8 hands", "Max of 10 hands");
	add_option("Gameover", "target_score",
		   "How many points does each team need to win?",
		   5,
		   2,
		   "Game to 100", "Game to 250",
		   "Game to 500", "Game to 1000", "Unending game");
	add_option("Gameover", "low_score_loses",
		   "Does dropping to -200 points cause an automatic loss?",
		   1, 1, "Forfeit at -200 points");
	add_option("Gameover", "variant301",
		   "When the game ends the highest score below 301 wins.",
		   1, 0, "'301' variant");
	add_option("Bidding", "bid_variant",
		   "These special bidding variations change the game "
		   "considerably.",
		   3, 0,
		   "Regular bidding rules.",
		   "'Whiz' bidding - bid the number of spades you hold.",
		   "'DN' bidding - you must bid double nil.");
	add_option("Bidding", "minimum_bid",
		   "What is the minimum bid that each team must meet?",
		   5, 0,
		   "Minimum bid 0", "Minimum bid 1",
		   "Minimum bid 2", "Minimum bid 3", "Minimum bid 4");
	add_option("Bidding", "unmakeable_bids",
		   "Allows unmakeably high bids of over thirteen.",
		   1, 0, "Allow team bids of over thirteen?");
	add_option("Bidding", "zero_bids",
		   "Whether a bid of zero (non-nil) is allowed.",
		   1, 1, "Allow zero bids?");
	add_option("Nil", "nil_tricks_count",
		   "How are tricks taken by the nil bidder scored? Do they\n"
		   "count toward the partner's bid, do they give overtricks\n"
		   "(+1 points each), or do they count only as bags (-100\n"
		   "for ten of them)?",
		   NIL_TRICKS_LAST, NIL_TRICKS_NO_POINTS,
		   "Nil tricks count toward the partner's bid",
		   "Nil tricks count as overtricks and bags.",
		   "Nil tricks count only as bags", "bags");

	game_get_options();
}

static int spades_handle_option(char *option, int value)
{
	if (strcmp("nil_value", option) == 0) {
		GSPADES.nil_value = 50 * value;
	} else if (strcmp("max_hands", option) == 0) {
		switch ((enum handlimit_option)value) {
		case HANDS_ONE:
			game.max_hands = 1;
			break;
		case HANDS_THREE:
			game.max_hands = 3;
			break;
		case HANDS_SIX:
			game.max_hands = 6;
			break;
		case HANDS_EIGHT:
			game.max_hands = 8;
			break;
		case HANDS_TEN:
			game.max_hands = 10;
			break;
		case HANDS_UNLIMITED:
			game.max_hands = MAX_HANDS;
			break;
		}
	} else if (strcmp("target_score", option) == 0) {
		switch (value) {
		case 0:
			game.target_score = 100;
			break;
		case 1:
			game.target_score = 250;
			break;
		case 2:
			game.target_score = 500;
			break;
		case 3:
			game.target_score = 1000;
			break;
		case 4:
			game.target_score = MAX_TARGET_SCORE;
			break;
		default:
			break;
		}
	} else if (strcmp("low_score_loses", option) == 0) {
		if (value == 0)
			game.forfeit_score = 0;
		else
			game.forfeit_score = -200;
	} else if (strcmp("minimum_bid", option) == 0) {
		GSPADES.minimum_team_bid = value;
	} else if (strcmp("double_nil", option) == 0) {
		GSPADES.double_nil_value = 100 * value;
	} else if (strcmp("nil_tricks_count", option) == 0) {
		GSPADES.nil_tricks_count = value;
	} else if (strcmp("unmakeable_bids", option) == 0) {
		GSPADES.unmakeable_bids = value;
	} else if (strcmp("zero_bids", option) == 0) {
		GSPADES.zero_bids = value;
	} else if (strcmp("bid_variant", option) == 0) {
		GSPADES.bid_variant = value;
	} else if (strcmp("variant301", option) == 0) {
		GSPADES.variant301 = value;
	} else {
		return game_handle_option(option, value);
	}
	return 0;
}

static char *spades_get_option_text(char *buf, int bufsz, char *option,
				    int value)
{
	buf[0] = '\0';

	if (strcmp(option, "nil_value") == 0) {
		if (value == 0)
			snprintf(buf, bufsz, "There are no nil bids.");
		else
			snprintf(buf, bufsz, "Nil is worth %d points.",
				 50 * value);
	} else if (strcmp(option, "target_score") == 0) {
		if (game.target_score == MAX_TARGET_SCORE)
			snprintf(buf, bufsz, "There is no points limit.");
		else
			snprintf(buf, bufsz,
				 "The game is being played to %d.",
				 game.target_score);
	} else if (strcmp(option, "max_hands") == 0) {
		switch ((enum handlimit_option)value) {
		case HANDS_ONE:
			snprintf(buf, bufsz,
				 "Only one hand will be played.");
			break;
		case HANDS_THREE:
			snprintf(buf, bufsz, "A maximum of 3 hands will "
				 "be played.");
			break;
		case HANDS_SIX:
			snprintf(buf, bufsz, "A maximum of 6 hands will "
				 "be played.");
			break;
		case HANDS_EIGHT:
			snprintf(buf, bufsz, "A maximum of 8 hands will "
				 "be played.");
			break;
		case HANDS_TEN:
			snprintf(buf, bufsz, "A maximum of 10 hands will "
				 "be played.");
			break;
		case HANDS_UNLIMITED:
			snprintf(buf, bufsz,
				 "There is no limit on the number"
				 " of hands to be played.");
			break;
		}
	} else if (strcmp(option, "low_score_loses") == 0) {
		if (value == 0)
			snprintf(buf, bufsz,
				 "Game is not forfeit at -200 points.");
		else
			snprintf(buf, bufsz,
				 "Game is forfeit at -200 points.");
	} else if (strcmp(option, "minimum_bid") == 0)
		snprintf(buf, bufsz, "The minimum team bid is %d.",
			 GSPADES.minimum_team_bid);
	else if (strcmp(option, "double_nil") == 0) {
		if (value == 0)
			snprintf(buf, bufsz,
				 "There is no blind (double) nil.");
		else
			snprintf(buf, bufsz,
				 "Blind (double) nil is worth %d.",
				 value == 1 ? 100 : 200);
	} else if (strcmp(option, "nil_tricks_count") == 0) {
		switch (value) {
		case NIL_TRICKS_COUNT:
			snprintf(buf, bufsz,
				 "A nil player's tricks count toward the "
				 "partner's bid.");
			break;
		case NIL_TRICKS_DONT_COUNT:
			snprintf(buf, bufsz,
				 "A nil player's tricks do not count "
				 "toward the partner's bid.");
			break;
		case NIL_TRICKS_NO_POINTS:
			snprintf(buf, bufsz,
				 "A nil player's tricks do not count "
				 "toward the partner's bid and give no "
				 "points.");
			break;
		}
	} else if (strcmp(option, "unmakeable_bids") == 0) {
		if (value != 0) {
			snprintf(buf, bufsz,
				 "Bids of over thirteen are allowed.");
		} else {
			snprintf(buf, bufsz,
				 "Thirteen is the highest bid allowed.");
		}
	} else if (strcmp(option, "zero_bids") == 0) {
		if (value != 0) {
			snprintf(buf, bufsz, "A bid of zero is allowed.");
		} else {
			snprintf(buf, bufsz,
				 "The lowest bid allowed is one.");
		}
	} else if (strcmp(option, "variant301") == 0) {
		if (value != 0) {
			snprintf(buf, bufsz,
				 "'301' variant - highest score below "
				 "301 wins.");
		}
	} else if (strcmp(option, "bid_variant") == 0) {
		switch ((enum bid_variant)value) {
		case BID_NORMAL:
			snprintf(buf, bufsz, "Regular bidding rules.");
			break;
		case BID_WHIZ:
			snprintf(buf, bufsz,
				 "'Whiz'  bidding - bid the number of spades "
				 "you hold.");
			break;
		case BID_DN_ONLY:
			snprintf(buf, bufsz,
				 "'DN' bidding - everyone must bid dnil.");
			break;
		}
	} else {
		return game_get_option_text(buf, bufsz, option, value);
	}
	return buf;
}

static void spades_start_bidding(void)
{
	game_start_bidding();
	game.bid_total = game.num_players;

	if (GSPADES.bid_variant == BID_DN_ONLY) {
		GSPADES.double_nil_value = 200;
		/* HACK: enforce dnil worth 200 with this variant. */
	}

	/* With blind bids, you first make a blind bid then a regular one. */
	if (GSPADES.double_nil_value > 0)
		game.bid_total += game.num_players;
}

static void spades_get_bid(void)
{
	int i;
	/* partner's bid (value) */
	struct game_player_t *partner =
	    &game.players[(game.next_bid + 2) % 4];
	char pard = partner->bid.sbid.val;

	if (GSPADES.double_nil_value > 0 && (game.bid_count % 2 == 0)) {
		/* A "blind" bid - you must first choose to bid blind (or
		   not). */

		/* TODO: make sure partner made minimum bid */
		if (GSPADES.bid_variant != BID_DN_ONLY) {
			add_sbid(NO_BID_VAL, NO_SUIT, SPADES_NO_BLIND);
		}
		if (partner->bid_count == 0
		    || pard >= GSPADES.minimum_team_bid
		    || GSPADES.bid_variant == BID_DN_ONLY) {
			add_sbid(NO_BID_VAL, NO_SUIT, SPADES_DNIL);
		}
	} else {
		/* A regular bid */
		int numspades = 0;

		for (i = 0; i < game.hand_size; i++) {
			if (game.seats[game.next_bid].hand.cards[i].suit
			    == SPADES) {
				numspades++;
			}
		}

		for (i = 0; i <= game.hand_size; i++) {
			if (partner->bid_count > 0 &&
			    pard + i < GSPADES.minimum_team_bid) {
				/* the second bidder on each team must make
				   sure the minimum bid count is met */
				continue;
			}
			if (!GSPADES.unmakeable_bids
			    && pard + i > game.hand_size) {
				/* Unless the option is set, team bids cannot
				   exceed thirteen. */
				continue;
			}
			if (!GSPADES.zero_bids && i == 0) {
				/* If this option is not set, zero-bids
				   aren't allowed. */
				continue;
			}
			if (GSPADES.bid_variant == BID_WHIZ
			    && i != numspades) {
				/* In the whiz variant you may only bid the
				   number of spades you are holding. */
				continue;
			}
			add_sbid(i, NO_SUIT, SPADES_BID);
		}

		/* "Nil" bid */
		/* If you're the first bidder, you can bid nil - your partner 
		   will be forced up to the minimum bid.  If you're the
		   second bidder, you can't bid nil unless your partner has
		   already met the minimum. */
		if (GSPADES.nil_value > 0
		    && (game.bid_count < 2
			|| pard >= GSPADES.minimum_team_bid))
			add_sbid(NO_BID_VAL, NO_SUIT, SPADES_NIL);
	}

	/* TODO: other specialty bids */
	request_client_bid(game.next_bid);
	spd_broadcast_scoredata();
}


static void spades_handle_bid(player_t p, bid_t bid)
{
	assert(game.next_bid == p);

	/* Regular bids don't need any special handling; however blind bids
	   do.  Note the bid_count is odd here since the bid_count has
	   already been updated before handle_bid() is called. */
	if (GSPADES.double_nil_value > 0 && (game.bid_count % 2 == 1)) {
		GSPADES.show_hand[p] = 1;
		spades_send_hand(p, p);
		if (bid.sbid.spec != SPADES_NO_BLIND)
			game.bid_count++;	/* skip regular bid */
	}
}

static void spades_next_bid(void)
{
	int next;
	if (game.bid_count == 0)
		game.next_bid = (game.dealer + 1) % game.num_players;
	else {
		if (GSPADES.double_nil_value == 0)
			next = 1;	/* one bid per person */
		else
			/* bid_count has already been incremented. */
			next = (game.bid_count % 2 == 0);
		if (next)
			game.next_bid =
			    (game.next_bid + 1) % game.num_players;
	}
}

static void spades_get_play(player_t p)
{
	game_get_play(p);
	spd_broadcast_scoredata();
}

static int spades_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == SPADES_NIL)
		return snprintf(buf, buf_len, "Nil");
	if (bid.sbid.spec == SPADES_DNIL)
		return snprintf(buf, buf_len, "Dnil");
	if (bid.sbid.spec == SPADES_NO_BLIND)
		return snprintf(buf, buf_len, "Show cards");
	assert(bid.sbid.val >= 0 && bid.sbid.val <= 13);
	return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
}

static int spades_get_bid_desc(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == SPADES_NIL)
		return snprintf(buf, buf_len,
				"Nil - contract to take no tricks");
	if (bid.sbid.spec == SPADES_DNIL)
		return snprintf(buf, buf_len,
				"Blind nil - blind contract to take no tricks");
	if (bid.sbid.spec == SPADES_NO_BLIND)
		return snprintf(buf, buf_len, "Show me the hand!");

	/* FIXME: show more stuff here, like team contract, etc. */
	return snprintf(buf, buf_len, "Contract to take %d tricks",
			(int)bid.sbid.val);
}

static void spades_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	clear_player_message(p);
	add_player_rating_message(p);
	add_player_score_message(p);

	if (game.state == STATE_NOTPLAYING)
		return;

	add_player_message(s, "Bags: %d\n", BAGS(p % 2));

	/* we show both the individual and team contract, if applicable.
	   But is this really the best way to tell if it's applicable? */
	if (game.state != STATE_NEXT_BID
	    && game.state != STATE_WAIT_FOR_BID) {
		char bid_text[512];
#if 0
		int contract =
		    game.players[p].bid.sbid.val +
		    game.players[(p + 2) % 4].bid.sbid.val;
#endif
		game.data->get_bid_text(bid_text, sizeof(bid_text),
					game.players[p].bid);
		if (*bid_text) {
#if 0	/* Disabled - this is deemed too confusing */
			add_player_message(s, "Bid: %s (%d)\n", bid_text,
					   contract);
#else
			add_player_message(s, "Bid: %s\n", bid_text);
#endif
		}
	} else
		add_player_bid_message(p);

	add_player_tricks_message(p);
	add_player_action_message(p);
}

static void spades_sync_player(player_t p)
{
	spd_send_scoredata(p);
}

static void spades_deal_hand(void)
{
	int shown = GSPADES.double_nil_value == 0;
	seat_t s;

	/* initialize all "shown hand" variables.  Setting this to 1 means
	   the hand is always shown; setting it to 0 means it's not shown.
	   This is used for blind bids, when the hand must be hidden until
	   the blind bid is made. */
	for (s = 0; s < game.num_seats; s++)
		GSPADES.show_hand[s] = shown;

	game_deal_hand();
}

static void spades_end_hand(void)
{
	player_t p;
	int x;
#define TRICKS_COUNT(pp) (GSPADES.nil_tricks_count == NIL_TRICKS_COUNT \
			  || (game.players[(pp)].bid.sbid.spec != SPADES_NIL \
			      && game.players[(pp)].bid.sbid.spec != SPADES_DNIL))

	for (p = 0; p < 2; p++) {
		int tricks, bid, score = 0;
		int bid1, bid2;

		/* Count points for contract. */
		bid1 = ((game.players[p].bid.sbid.spec == SPADES_BID)
			? game.players[p].bid.sbid.val : 0);
		bid2 = ((game.players[p + 2].bid.sbid.spec == SPADES_BID)
			? game.players[p + 2].bid.sbid.val : 0);
		bid = bid1 + bid2;
		tricks = 0;
		if (TRICKS_COUNT(p))
			tricks += game.players[p].tricks;
		if (TRICKS_COUNT(p + 2))
			tricks += game.players[p + 2].tricks;
		if (tricks >= bid) {
			score += 10 * bid;
			BAGS(p) += (tricks - bid);
			score += (tricks - bid);
		} else {
			score = -10 * bid;
		}

		/* Count bags on nil/dnil bids. */
		for (x = 0; x < 2; x++) {
			player_t p2 = p + 2 * x;

			if (game.players[p2].bid.sbid.spec != SPADES_NIL
			    && game.players[p2].bid.sbid.spec !=
			    SPADES_DNIL) {
				continue;
			}

			if (!TRICKS_COUNT(p2)) {
				BAGS(p) += game.players[p2].tricks;
				if (GSPADES.nil_tricks_count
				    == NIL_TRICKS_DONT_COUNT) {
					score += game.players[p2].tricks;
				}
			}
		}
		while (BAGS(p) >= 10) {
			/* you lose 100 points for 10 overtricks */
			BAGS(p) -= 10;
			score -= 100;
		}
		ggz_debug(DBG_GAME, "Team %d bid %d, took %d, earned %d.",
			  (int)p, bid, tricks, score);
		change_score(p, score);
	}
	for (p = 0; p < 4; p++) {
		if (game.players[p].bid.sbid.spec == SPADES_NIL) {
			int score =
			    (game.players[p].tricks ==
			     0 ? GSPADES.nil_value : -GSPADES.nil_value);
			ggz_debug(DBG_GAME,
				  "Player %d/%s earned %d for going nil.",
				  p, get_player_name(p), score);
			change_score(p % 2, score);
		}
		if (game.players[p].bid.sbid.spec == SPADES_DNIL) {
			int score =
			    (game.players[p].tricks == 0 ?
			     GSPADES.double_nil_value :
			     -GSPADES.double_nil_value);
			ggz_debug(DBG_GAME,
				  "Player %d/%s earned %d for double nil.",
				  p, get_player_name(p), score);
			change_score(p % 2, score);
		}
		assert(game.players[p].bid.sbid.spec != SPADES_NO_BLIND);
		set_player_message(p);
	}

	spd_broadcast_scoredata();
}

static void spades_start_game(void)
{
	game_start_game();
	spd_broadcast_scoredata();
}

static void spades_handle_gameover(void)
{
	if (GSPADES.variant301) {
		/* Quick hack: for the 301 variant just negate any scores
		   higher than 301.  This means ties could be allowed in
		   some cases (not sure if that is bad or not). */
		/* This also means scores will actually be displayed at the
		   client as negative for these players.  And incidentally
		   a score of 302 will incorrectly beat a score of -303. */
		teams_iterate(t) {
			if (game.teams[t].score.score > 301) {
				game.teams[t].score.score
				    = -game.teams[t].score.score;
			}
		}
		teams_iterate_end;
	}

	game_handle_gameover();
}

static void spades_send_hand(player_t p, seat_t s)
{
	/* in most cases, we want to reveal the hand only to the player who
	   owns it. */
	bool show_fronts = (p >= 0
			    && game.players[p].seat == s
			    && GSPADES.show_hand[s]);

	send_hand(p, s, show_fronts, TRUE);
}
