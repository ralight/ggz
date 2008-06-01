<?php

function games_intro()
{
	global $database;

	$res = $database->exec("SELECT game, COUNT(game) FROM matches " .
		"GROUP BY game ORDER BY COUNT(game) DESC LIMIT 1", NULL);
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, 0, "count");
		$game = $database->result($res, 0, "game");

		echo "<img src='/db/ggzicons/games/$game.png' width=16 height=16>\n";
		echo "The most popular game is ";
		echo "<a href='/db/games/?lookup=$game'>$game</a> ";
		echo "with $count finished matches.";
	else :
		echo "No game has ever been played on this server.";
	endif;
}

function match_intro()
{
	global $database;

	$res = $database->exec("SELECT winner, COUNT(winner) FROM matches, users " .
		"WHERE matches.winner = users.handle " .
		"GROUP BY winner ORDER BY COUNT(winner) DESC LIMIT 1", NULL);
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, 0, "count");
		$player = $database->result($res, 0, "winner");

		$p = new Player($player);
		$p->icon();

		echo "The most successful player is ";
		echo "<a href='/db/players/?lookup=$player'>$player</a> ";
		echo "who won $count matches.";
	else :
		echo "No match has ever happened on this server.";
	endif;
}

function tournament_intro()
{
	global $database;

	$res = $database->exec("SELECT game, COUNT(game) FROM tournaments " .
		"GROUP BY game ORDER BY COUNT(game) DESC LIMIT 1", NULL);
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, 0, "count");
		$game = $database->result($res, 0, "game");

		echo "<img src='/db/ggzicons/games/$game.png' width=16 height=16>\n";
		echo "The most popular game in tournaments is ";
		echo "<a href='/db/games/?lookup=$game'>$game</a> ";
		echo "with $count occurrences.";
	else :
		echo "No tournament has ever been organized on this server.";
	endif;
}

function player_intro()
{
	global $database;

	$res = $database->exec("SELECT handle, COUNT(handle) FROM matchplayers " .
		"WHERE HANDLE NOT LIKE '%|AI' GROUP BY handle " .
		"ORDER BY COUNT(handle) DESC LIMIT 1", NULL);
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, 0, "count");
		$player = $database->result($res, 0, "handle");

		$p = new Player($player);
		$p->icon();

		echo "The most active player is ";
		echo "<a href='/db/players/?lookup=" . htmlurl($player) . "'>" . html($player) . "</a> ";
		echo "with $count games played.";
	else :
		echo "The players just seem to chat instead of playing games.";
	endif;
}

function team_intro()
{
	global $database;

	$res = $database->exec("SELECT teamname, COUNT(teamname) FROM teammembers " .
		"GROUP BY teamname ORDER BY COUNT(teamname) DESC LIMIT 1", NULL);
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, 0, "count");
		$team = $database->result($res, 0, "teamname");

		echo "The largest team is ";
		echo "<a href='/db/teams/?lookup=$team'>$team</a> ";
		echo "with $count members.";
	else :
		echo "No teams were founded as of yet.";
	endif;
}

?>
