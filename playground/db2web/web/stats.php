<?php

function stats_players($id, $lookup)
{
	$res = pg_exec($id, "SELECT * FROM stats WHERE handle = '$lookup'");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$game = pg_result($res, $i, "game");
		$wins = pg_result($res, $i, "wins");
		$losses = pg_result($res, $i, "losses");
		$ties = pg_result($res, $i, "ties");
		$forfeits = pg_result($res, $i, "forfeits");
		$rating = pg_result($res, $i, "rating");
		$ranking = pg_result($res, $i, "ranking");
		$highscore = pg_result($res, $i, "highscore");

		echo "<img src='ggzicons/games/$game.png' width=16 height=16>\n";
		echo "<a href='$SCRIPT_NAME?lookup=$game&type=game'>$game</a>:<br>\n";
		echo "$lookup achieved <b>$wins</b> wins, <b>$losses</b> losses, <b>$ties</b> ties, <b>$forfeits</b> forfeits.<br>\n";
		echo "His/her rating is <b>$rating</b> (ranking him/her <b>$ranking</b> th place),\n";
		echo "with highest score being <b>$highscore</b>.<br>\n";
		echo "<br>\n";
	}
	if (pg_numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	endif;
}

function stats_games($id, $lookup)
{
	global $ggzuser;

	$res = pg_exec($id, "SELECT * FROM stats WHERE game = '$lookup'");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$handle = pg_result($res, $i, "handle");
		$wins = pg_result($res, $i, "wins");
		$losses = pg_result($res, $i, "losses");
		$ties = pg_result($res, $i, "ties");
		$forfeits = pg_result($res, $i, "forfeits");
		$rating = pg_result($res, $i, "rating");
		$ranking = pg_result($res, $i, "ranking");
		$highscore = pg_result($res, $i, "highscore");

		$handlecaption = $handle;
		$pic = "player.png";
		if ($handle == $ggzuser) :
			$handlecaption = "<b>$handle</b>";
			$pic = "you.png";
		endif;

		echo "<img src='ggzicons/players/$pic' width=16 height=16>\n";
		echo "<a href='$SCRIPT_NAME?lookup=$handle&type=player'>$handlecaption</a>\n";
		echo "achieved <b>$wins</b> wins, <b>$losses</b> losses, <b>$ties</b> ties, <b>$forfeits</b> forfeits.<br>\n";
		echo "His/her rating is <b>$rating</b> (ranking him/her <b>$ranking</b> th place),\n";
		echo "with highest score being <b>$highscore</b>.<br>\n";
		echo "<br>\n";
	}
}

function stats_statistics($id)
{
	$res = pg_exec($id, "SELECT COUNT(*) FROM users");
	$count = pg_result($res, 0, "count");
	echo "<b>$count</b> players have registered with this server.<br>\n";

	$res = pg_exec($id, "SELECT COUNT(*) FROM stats");
	$count = pg_result($res, 0, "count");
	echo "<b>$count</b> records are available in the ranking database.<br>\n";

}

?>

