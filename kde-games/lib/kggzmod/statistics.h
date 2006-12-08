#ifndef KGGZMOD_STATISTICS_H
#define KGGZMOD_STATISTICS_H

namespace KGGZMod
{

class StatisticsPrivate;

/**
 * @short Statistics of a player in an online game.
 *
 * Depending on the game type, a game might support a simple
 * record-style statistics, for which the \ref wins, \ref losses,
 * \ref ties and \ref forfeits can be queried.
 *
 * Other games support \ref highscore, \ref ranking and
 * \ref rating values.
 *
 * Which type is supported by the current game can be queried
 * on the statistics object as well.
 *
 * There is no constructor for the class since statistics can
 * only be found on actual \ref Player objects.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class Statistics
{
	friend class ModulePrivate;

	public:
		~Statistics();

		/**
		 * The number of times a player has won this game.
		 *
		 * The number of wins are only valid if \ref hasRecord returns \b true.
		 *
		 * @return number of wins
		 */
		int wins() const;

		/**
		 * The number of times a player has lost this game.
		 *
		 * The number of losses are only valid if \ref hasRecord returns \b true.
		 *
		 * @return number of losses
		 */
		int losses() const;

		/**
		 * The number of times a player has tied with another one.
		 *
		 * This is when the game ended and there was no clear winner.
		 *
		 * The number of ties are only valid if \ref hasRecord returns \b true.
		 *
		 * @return number of ties
		 */
		int ties() const;

		/**
		 * The number of times a player has forfeited.
		 *
		 * This is when a player leaves the game without properly abandoning the seat.
		 *
		 * The number of forfeits are only valid if \ref hasRecord returns \b true.
		 *
		 * @return number of forfeits
		 */
		int forfeits() const;

		/**
		 * The rating of a player.
		 *
		 * This is the rounded number of a calculation which depends on the 
		 * game type. For example, chess games might use Elo-style ratings.
		 *
		 * The rating is only valid if \ref hasRating returns \b true.
		 *
		 * @return rating for the player in this game type
		 */
		int rating() const;

		/**
		 * The ranking of a player.
		 *
		 * This is the position in a ranking list, where 1 is the 1st place
		 * and all others are below.
		 *
		 * The ranking is only valid if \ref hasRanking returns \b true.
		 *
		 * @return ranking for the player in this game type
		 */
		int ranking() const;

		/**
		 * The highscore of a player.
		 *
		 * In highscore-based games, this determines the best for each player.
		 *
		 * The highscore is only valid if \ref hasHighscore returns \b true.
		 *
		 * @return highscore for the player in this game type
		 */
		int highscore() const;

		/**
		 * Determines if a game supports records.
		 *
		 * @return \b true if it supports records, \b false otherwise
		 */
		bool hasRecord() const;

		/**
		 * Determines if a game supports ratings.
		 *
		 * @return \b true if it supports ratings, \b false otherwise
		 */
		bool hasRating() const;

		/**
		 * Determines if a game supports rankings.
		 *
		 * @return \b true if it supports rankings, \b false otherwise
		 */
		bool hasRanking() const;

		/**
		 * Determines if a game supports highscores.
		 *
		 * @return \b true if it supports highscores, \b false otherwise
		 */
		bool hasHighscore() const;

	private:
		Statistics();
		StatisticsPrivate *d;
		void init(StatisticsPrivate *x);
};

}

#endif

