// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

/** @brief Scoring options for players
 *
 * When a game has finished, each player can be assigned an object from this
 * class to specify the outcome of the game.
 *
 * @see GGZDMod::Handler::reportSavegame
 */
public class Score
{
	private int result;
	private int score;
	private int team;

	/** @brief Constructor
	 *
	 * Each player is either a winner (RESULT_WIN) or a loser (RESULT_LOST).
	 * Some games also end with some players vanishing (RESULT_FORFEIT) or all
	 * players being of equal luck (RESULT_TIE).
	 *
	 * @param result Game result as per GGZDMod::Protocol
	 * @param score Highscore, if applicable
	 * @param team Number of the team, or \b -1 if no teams have been defined
	 */
	public Score(int result, int score, int team)
	{
		this.result = result;
		this.score = score;
		this.team = team;
	}

	/** @brief Return the highscore
	 *
	 * @return Highscore achieved by a player in a single game
	 */
	public int getScore()
	{
		return this.score;
	}

	/** @brief Return the game outcome
	 *
	 * @return Game outcome as per GGZDMod::Protocol
	 */
	public int getResult()
	{
		return this.result;
	}

	/** @brief Return the player's team
	 *
	 * @return Player team number, or \b -1 if no teams have been defined
	 */
	public int getTeam()
	{
		return this.team;
	}
}

