/* 
 * File: ggz_stats.c
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 9/4/01
 * Desc: GGZ game module stat functions
 * $Id: ggz_stats.c 4131 2002-05-02 04:29:21Z jdorje $
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ggzdmod.h"
#include "ggz_stats.h"

extern int num_seats;
extern struct ggzd_seat_t *seat_data;

/* FIXME: using static values is no good with GGZdmod's object-orient system. 
 */
static int *ratings = NULL;
static int teams = 0;
static int *team_list = NULL;
static double *winners = NULL;
static GGZRatingSystem rating_system = GGZ_ELO_RATING;
static const char *module = NULL;

/* FIXME: this table is about the ugliest thing ever.  It holds the CDF's of
   the normalized normal distribution. */
static double ztable[301] = {
	0.0013498974275996112,
	0.0013948866316060848,
	0.0014412413183508832,
	0.0014889981559605414,
	0.001538194637014656,
	0.001588869092024936,
	0.0016410607029800905,
	0.0016948095169527777,
	0.001750156459762453,
	0.0018071433496901768,
	0.0018658129112387178,
	0.0019262087889329593,
	0.0019883755611550535,
	0.002052358754007555,
	0.0021182048551989241,
	0.0021859613279451295,
	0.0022556766248800209,
	0.002327400201968588,
	0.0024011825324152247,
	0.0024770751205606678,
	0.0025551305157595627,
	0.0026354023262312176,
	0.0027179452328760512,
	0.0028028150030493526,
	0.0028900685042839713,
	0.0029797637179544423,
	0.0030719597528726106,
	0.0031667168588071504,
	0.0032640964399167638,
	0.0033641610680892331,
	0.0034669744961752791,
	0.0035726016711090103,
	0.0036811087469041937,
	0.003792563097516688,
	0.0039070333295626036,
	0.0040245892948822526,
	0.0041453021029385084,
	0.0042692441330391961,
	0.0043964890463726869,
	0.0045271117978451514,
	0.0046611886477079811,
	0.0047987971729647194,
	0.0049400162785444568,
	0.0050849262082311442,
	0.005233608555335667,
	0.0053861462730993015,
	0.0055426236848157839,
	0.0057031264936595605,
	0.0058677417922075592,
	0.0060365580716419398,
	0.0062096652306207201,
	0.0063871545838029564,
	0.0065691188700164327,
	0.00675565226005298,
	0.0069468503640799373,
	0.0071428102386527081,
	0.0073436303933163138,
	0.0075494107967811197,
	0.0077602528826604122,
	0.0079762595547552273,
	0.0081975351918731065,
	0.0084241856521673486,
	0.0086563182769826552,
	0.0088940418941937938,
	0.0091374668210232324,
	0.0093867048663250907,
	0.0096418693323204185,
	0.0099030750157714231,
	0.010170438208581323,
	0.01044407669780506,
	0.010724109765059997,
	0.011010658185321376,
	0.01130384422509112,
	0.011603791639926586,
	0.011910625671316843,
	0.012224473042894923,
	0.012545461955972614,
	0.012873722084387529,
	0.01320938456864984,
	0.0135525820093787,
	0.013903448460015899,
	0.01426211941880845,
	0.014628731820047924,
	0.015003424024558676,
	0.015386335809424667,
	0.015777608356947082,
	0.016177384242823556,
	0.016585807423542764,
	0.017003023222986247,
	0.017429178318231431,
	0.017864420724550745,
	0.018308899779600374,
	0.018762766126796393,
	0.019226171697872219,
	0.019699269694617383,
	0.0201822145697933,
	0.020675162007226688,
	0.021178268901080211,
	0.021691693334300266,
	0.022215594556245999,
	0.022750132959500013,
	0.023295467792401992,
	0.023851764485976978,
	0.024419185453399439,
	0.024997895303140116,
	0.025588059631423632,
	0.026189844992999645,
	0.026803418870745443,
	0.027428949644120293,
	0.028066606556483631,
	0.028716559681294385,
	0.029378979887203005,
	0.030054038802049154,
	0.03074190877577504,
	0.031442762842267435,
	0.032156774680134892,
	0.032884118572433452,
	0.033624969365345592,
	0.034379502425825104,
	0.03514789359821513,
	0.035930319159845292,
	0.036726955775621406,
	0.03753798045161244,
	0.038363570487644771,
	0.03920390342891289,
	0.040059157016615887,
	0.040929509137628106,
	0.041815137773216382,
	0.042716220946811212,
	0.043632936670844613,
	0.044565462892666607,
	0.045513977439549824,
	0.046478657962796821,
	0.047459681880963556,
	0.048457226322209723,
	0.049471468065793944,
	0.050502583482726904,
	0.05155074847559854,
	0.052616138417595104,
	0.053698928090725517,
	0.054799291623271407,
	0.055917402426483021,
	0.057053433130537734,
	0.058207555519782628,
	0.059379940467282288,
	0.06057075786869176,
	0.061780176575479129,
	0.063008364327519828,
	0.064255487685086599,
	0.065521711960259965,
	0.066807201147784379,
	0.06811211785539717,
	0.069436623233655514,
	0.070780876905290446,
	0.072145036894116166,
	0.073529259553522797,
	0.074933699494582784,
	0.076358509513802353,
	0.077803840520547174,
	0.079269841464176316,
	0.080756659260914632,
	0.082264438720498823,
	0.083793322472628884,
	0.085343450893260786,
	0.086914962030774756,
	0.088507991532053798,
	0.090122672568510254,
	0.091759135762094657,
	0.093417509111326291,
	0.095097917917381369,
	0.096800484710277135,
	0.098525329175190968,
	0.10027256807895296,
	0.10204231519675117,
	0.10383468123908968,
	0.10564977377903884,
	0.10748769717981899,
	0.10934855252275771,
	0.11123243753566175,
	0.11313944652164465,
	0.11506967028845272,
	0.11702319607832912,
	0.11900010749846002,
	0.12100048445204298,
	0.1230244030700216,
	0.12507193564352681,
	0.12714315055706898,
	0.12923811222252168,
	0.13135688101394061,
	0.13349951320325965,
	0.13566606089690608,
	0.13785657197337792,
	0.14007109002182488,
	0.1423096542816758,
	0.14457229958335321,
	0.14685905629011764,
	0.14916995024108309,
	0.15150500269544415,
	0.15386423027795648,
	0.15624764492571003,
	0.15865525383623708,
	0.1610870594169927,
	0.1635430592362484,
	0.16602324597543822,
	0.16852760738299372,
	0.17105612622970845,
	0.1736087802656674,
	0.17618554217877852,
	0.17878637955494342,
	0.18141125483990161,
	0.18406012530278404,
	0.18673294300140969,
	0.18942965474935874,
	0.192150202084855,
	0.19489452124148976,
	0.19766254312081705,
	0.2004541932668526,
	0.2032693918425032,
	0.20610805360795725,
	0.20897008790106242,
	0.21185539861971758,
	0.21476388420630449,
	0.21769543763418348,
	0.22064994639627722,
	0.22362729249576385,
	0.22662735243890197,
	0.22964999723000723,
	0.23269509236859903,
	0.23576249784873582,
	0.23885206816055621,
	0.24196365229403949,
	0.24509709374500249,
	0.24825223052334316,
	0.25142889516354494,
	0.25462691473745036,
	0.25784611086931519,
	0.26108629975314901,
	0.26434729217234998,
	0.2676288935216391,
	0.27093090383129681,
	0.27425311779370587,
	0.27759532479220023,
	0.28095730893222093,
	0.28433884907477647,
	0.28773971887220517,
	0.29115968680623477,
	0.29459851622833394,
	0.29805596540234741,
	0.30153178754940763,
	0.30502573089511137,
	0.30853753871895068,
	0.31206694940598489,
	0.31561369650073978,
	0.31917750876331719,
	0.32275811022769929,
	0.3263552202622283,
	0.32996855363224042,
	0.33359782056483578,
	0.33724272681575818,
	0.34090297373836298,
	0.34457825835464639,
	0.34826827342831013,
	0.35197270753983279,
	0.35569124516351835,
	0.35942356674649178,
	0.36316934878960927,
	0.36692826393024869,
	0.37069998102694834,
	0.37448416524585493,
	0.37828047814894639,
	0.38208857778398969,
	0.38590811877619485,
	0.38973875242152484,
	0.39358012678161919,
	0.39743188678028851,
	0.40129367430153673,
	0.40516512828906587,
	0.40904588484721671,
	0.41293557734330005,
	0.41683383651126915,
	0.4207402905566856,
	0.42465456526292905,
	0.42857628409859905,
	0.43250506832605989,
	0.43644053711107383,
	0.44038230763347214,
	0.44432999519880867,
	0.44828321335094301,
	0.45224157398549714,
	0.45620468746413118,
	0.46017216272958139,
	0.46414360742140437,
	0.46811862799236997,
	0.47209682982544549,
	0.47607781735131321,
	0.4800611941663635,
	0.4840465631511035,
	0.48803352658892385,
	0.49202168628516352,
	0.49601064368641357,
	0.5
};

/* Looks like ELO uses a standard deviation of 200. */
#define STD_DEV (double)200.0
#define INCR (double)(1.0 / 100.0)

static double get_normal_cdf(double z)
{
	/* This may not be worth the trouble... */
	if (z < 0)
		return 1 - get_normal_cdf(-z);

	/* outside of our range */
	if (z > 3)
		return 1.0;

	/* There are 100 entries per std-dev, and they're in reverse order,
	   AND they're for the lower end of the CDF. */
	return 1 - ztable[300 - (int) (z * 100.0 + 0.5)];
}


static double elo_integrate_part(double val, double incr,
				 int num, int i, double *ratings)
{
	double prob, myrating = ratings[i];
	int j;

	prob = get_normal_cdf(val + incr) - get_normal_cdf(val);

	for (j = 0; j < num; j++) {
		double prob2;
		double ratingdiff = myrating - ratings[j];

		if (j == i)
			continue;

		prob2 = get_normal_cdf(val + ratingdiff);

		prob *= prob2;
	}

	return prob;
}

/* Returns ELO-style probability of winning determined by integration num is
   number of players i is player being considered ratings is ratings of all
   players */
static double elo_integrate_all(int num, int i, double *ratings)
{
	double prob = 0, p;

	for (p = -3; p < 3; p += INCR) {
		prob += elo_integrate_part(p, INCR, num, i, ratings);
	}

	return prob;
}

void elo_compute_expectations(GGZdMod * ggz, int num, int *ratings,
			      double *probs)
{
	double myratings[num];
	int i;
	double sum = 0;

	/* "normalize" the ratings */
	for (i = 0; i < num; i++) {
		myratings[i] = ratings[i];
		myratings[i] /= STD_DEV;
	}

	for (i = 0; i < num; i++) {
		probs[i] = elo_integrate_all(num, i, myratings);
		sum += probs[i];
	}

	ggzdmod_log(ggz, "GGZDMOD: ELO STATS: "
		    "Probabilities sum to %f; normalizing.", sum);
	for (i = 0; i < num; i++)
		probs[i] /= sum;
}


/* 
 * below here we have the interface functions
 */


/* it could be really bad if you changed systems mid-calculation. */
void ggzd_set_rating_system(GGZRatingSystem system)
{
	rating_system = system;
}


int read_ratings(GGZdMod * ggz)
{
	int num = ggzdmod_get_num_seats(ggz);
	char fname[128], lockname[128];
	int player;
	int handle;
	
	if (!module)
		return -1;
		
	/* FIXME: use database */
		
	snprintf(lockname, sizeof(lockname), "stats-lock-%s", module);
	while (mkdir(lockname, S_IRWXU) < 0) {
		fprintf(stderr, "Couldn't access stats lock.");
		sleep(1);
	}
	
	snprintf(fname, sizeof(fname), "stats-%s", module);
	handle = ggz_conf_parse(fname, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if (handle < 0) {
		if (rmdir(lockname) < 0)
			assert(0);
		return -1;
	}

	for (player = 0; player < num; player++) {
		GGZSeat seat = ggzdmod_get_seat(ggz, player);
		char player_name[128];
		
		if (seat.type != GGZ_SEAT_PLAYER &&
		    seat.type != GGZ_SEAT_BOT)
			continue;
		
		if (seat.type == GGZ_SEAT_PLAYER)
			snprintf(player_name, sizeof(player_name), "%s", seat.name);
		else
			snprintf(player_name, sizeof(player_name), "%s(AI)(%d)",
			         seat.name, seat.num);
			
		ratings[player] = ggz_conf_read_int(handle, "ratings",
		                                    player_name,
		                                    1500);
	}
	
	ggz_conf_cleanup(); /* Hope nobody else is using this! */
		
	if (rmdir(lockname) < 0)
		assert(0);
		
	return 0;
}

int write_ratings(GGZdMod * ggz)
{
	int num = ggzdmod_get_num_seats(ggz);
	char fname[128], lockname[128];
	int handle;
	int player;
	
	if (!module)
		return -1;
		
	/* FIXME: use database */
		
	snprintf(lockname, sizeof(lockname), "stats-lock-%s", module);
	while (mkdir(lockname, S_IRWXU) < 0) {
		fprintf(stderr, "Couldn't access stats lock.");
		sleep(1);
	}	
	
	snprintf(fname, sizeof(fname), "stats-%s", module);
	handle = ggz_conf_parse(fname, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if (handle < 0) {
		if (rmdir(lockname) < 0)
			assert(0);
		return -1;
	}

	for (player = 0; player < num; player++) {
		GGZSeat seat = ggzdmod_get_seat(ggz, player);
		char player_name[128];
		
		if (seat.type != GGZ_SEAT_PLAYER &&
		    seat.type != GGZ_SEAT_BOT)
			continue;
		
		if (seat.type == GGZ_SEAT_PLAYER)
			snprintf(player_name, sizeof(player_name), "%s", seat.name);
		else
			snprintf(player_name, sizeof(player_name), "%s(AI)(%d)",
			         seat.name, seat.num);
			
		ggz_conf_write_int(handle, "ratings", player_name, ratings[player]);
	}
	
	ggz_conf_commit(handle);
	ggz_conf_cleanup(); /* Hope nobody else is using this! */
		
	if (rmdir(lockname) < 0)
		assert(0);
		
	return 0;
}

int ggzd_get_rating(GGZdMod * ggz, int player)
{
	if (player < 0 || player >= ggzdmod_get_num_seats(ggz)) {
		ggzdmod_log(ggz, "GGZDMOD: ELO STATS: "
			    "ggzd_get_rating: invalid player %d.", player);
		return 0;
	}

	/* FIXME: right now we don't have persistent ratings since there's no 
	   access to the backend database.  So we just initialize everyone to 
	   1500 at the start of each table. Not sure how this will work with
	   people joining/leaving the table... */
	if (!ratings) {
		ratings = ggz_malloc(ggzdmod_get_num_seats(ggz) * sizeof(*ratings));
		if (read_ratings(ggz) < 0) {
			ggz_free(ratings);
			ratings = NULL; /* We may try again later...ugh. */
			return -1; /* ? */
		}
	}

	return ratings[player];
}

void ggzd_set_num_teams(GGZdMod * ggz, int num_teams)
{
	if (teams < 0 || teams >= ggzdmod_get_num_seats(ggz)) {
		ggzdmod_log(ggz, "GGZDMOD: ELO STATS: "
			    "ggzd_set_num_teams: invalid number %d.",
			    num_teams);
		return;
	}
	teams = num_teams;
}

void ggzd_set_team(GGZdMod * ggz, int player, int team)
{
	if (player < 0 || player >= ggzdmod_get_num_seats(ggz) || team < 0
	    || team >= teams) {
		ggzdmod_log(ggz, "GGZDMOD: ELO STATS: "
			    "ggzd_set_team: invalid params %d/%d.", player,
			    team);
		return;
	}

	if (!team_list) {
		int i, num = ggzdmod_get_num_seats(ggz);
		team_list = malloc(num * sizeof(*team_list));
		for (i = 0; i < num; i++)
			team_list[i] = -1;	/* no team */
	}

	team_list[player] = team;
}

void ggzd_set_game_winner(GGZdMod * ggz, int player, double score)
{
	if (player < 0 || player >= ggzdmod_get_num_seats(ggz) || score < 0.0
	    || score > 1.0) {
		ggzdmod_log(ggz, "GGZDMOD: ELO STATS: "
			    "ggzd_set_game_winner: invalid params %d/%f.",
			    player, score);
		return;
	}

	if (!winners) {
		int i, num = ggzdmod_get_num_seats(ggz);
		winners = malloc(num * sizeof(*winners));
		for (i = 0; i < num; i++)
			winners[i] = 0;	/* not a winner */
	}

	winners[player] = score;
}

/* NOTE Teams are handled like this: a team effectively has a rating equal to 
   the average of all players on that team.  This is put into ELO to
   determine the chance of each team winning.  However, the rating change of
   each player is dependent on that players's rating (or variance), not the
   team's, according to the standard ELO (or Glicko) formula. */

/* This function is pretty damned ugly. */
int ggzd_recalculate_ratings(GGZdMod * ggz)
{
	int i, num = teams > 0 ? teams : ggzdmod_get_num_seats(ggz);
	double sum = 0;

	double team_probs[num], team_scores[num];
	int team_ratings[num], team_sizes[num];

	/* check to see if all data is initted */
	if (!winners || !ratings || (teams && !team_list)) {
		ggzdmod_log(ggz, "GGZDMOD: ELO: ERROR: "
			    "ggzd_recalculate_ratings: invalid rating data.");
		return -1;
	}

	/* first lets initialize the team arrays */
	for (i = 0; i < num; i++) {
		team_ratings[i] = team_sizes[i] = team_scores[i] = 0;
	}
	for (i = 0; i < ggzdmod_get_num_seats(ggz); i++) {
		int team = teams > 0 ? team_list[i] : i;
		team_ratings[team] += ratings[i];
		team_sizes[team]++;
		if (team_scores[team] && team_scores[team] != winners[i]) {
			ggzdmod_log(ggz, "GGZDMOD: ELO: ERROR: "
				    "ggzd_recalculate_ratings: inconsistent team.");
			return -1;
		}
		team_scores[team] = winners[i];
	}
	for (i = 0; i < num; i++) {
		/* the team rating is the average of the individual ratings. */
		team_ratings[i] /= team_sizes[i];	/* FIXME */
		ggzdmod_log(ggz,
			    "GGZDMOD: ELO: " "Team rating for team %d is %d.",
			    i, team_ratings[i]);
		sum += team_scores[i];
	}

	/* Verify there is exactly 1 winner. */
	if (sum != 1.0) {
		/* FIXME: rounding error might cause this */
		ggzdmod_log(ggz, "GGZDMOD: ERROR: winner sums add to %f.",
			    sum);
		return -1;
	}

	/* Calculate the probability for each player to win, ELO-style. */
	elo_compute_expectations(ggz, num, team_ratings, team_probs);

	/* Debugging data */
	for (i = 0; i < ggzdmod_get_num_seats(ggz); i++) {
		int team = teams > 0 ? team_list[i] : i;
		ggzdmod_log(ggz, "GGZDMOD: ELO RATINGS: "
			    "Player %d has rating %d, expectation %f.", i,
			    team_ratings[team], team_probs[team]);
	}

	/* Calculate new ratings for all players. */
	for (i = 0; i < ggzdmod_get_num_seats(ggz); i++) {
		int team = teams > 0 ? team_list[i] : i;
		double K, diff;

		/* FIXME: this is the chess distribution; games should be
		   able to set their own. */
		if (ratings[i] < 2000)
			K = 30.0;
		else if (ratings[i] > 2400)
			K = 10.0;
		else
			K = 130.0 - (double) ratings[i] / 20.0;

		diff = K * (team_scores[team] - team_probs[team]);
		ratings[i] += (int) (diff + 0.5);
		ggzdmod_log(ggz, "GGZDMOD: ELO RATING: "
			    "Player %d has new rating %d (slope %f).", i,
			    ratings[i], K);
	}
	
	if (write_ratings(ggz) < 0) {
	
	}

	/* re-init the winners */
	memset(winners, 0, num * sizeof(*winners));

	return 0;		/* success */
}

void ggzd_set_module(const char* module_name)
{
	module = module_name;
}
