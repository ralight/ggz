<?php

function games_intro()
{
	global $database;

	$res = $database->exec("SELECT game, COUNT(game) FROM matches GROUP BY game ORDER BY COUNT(game) DESC LIMIT 1");
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, $i, "count");
		$game = $database->result($res, $i, "game");

		echo "<img src='/db/ggzicons/games/$game.png' width=16 height=16>\n";
		echo "The most popular game is <a href='/db/games/?lookup=$game'>$game</a> with $count finished matches.";
	endif;
}

function player_intro()
{
	global $database;

	$res = $database->exec("SELECT handle, COUNT(handle) FROM matchplayers " .
		"WHERE HANDLE NOT LIKE '%|AI' GROUP BY handle " .
		"ORDER BY COUNT(handle) DESC LIMIT 1");
	if (($res) && ($database->numrows($res))) :
		$count = $database->result($res, $i, "count");
		$player = $database->result($res, $i, "handle");

		echo "<img src='/db/ggzicons/players/player.png' width=16 height=16>\n";
		echo "The most active player is <a href='/db/players/?lookup=$player'>$player</a> with $count games played.";
	endif;
}

?>
