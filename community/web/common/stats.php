<?php

function stats_players($id, $lookup)
{
	if (!$lookup) return;

	$res = pg_exec($id, "SELECT * FROM stats WHERE lower(handle) = lower('$lookup')");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$game = pg_result($res, $i, "game");
		$wins = pg_result($res, $i, "wins");
		$losses = pg_result($res, $i, "losses");
		$ties = pg_result($res, $i, "ties");
		$forfeits = pg_result($res, $i, "forfeits");
		$rating = pg_result($res, $i, "rating");
		$rank = pg_result($res, $i, "ranking");
		$highscore = pg_result($res, $i, "highscore");

		$rating = (int)($rating);

		$res2 = pg_exec($id, "SELECT * FROM rankings WHERE game = '$game'");
		if (($res2) && (pg_numrows($res2) == 1)) :
			$method = pg_result($res2, 0, "method");
		else :
			$method = "wins/losses";
		endif;

		if ($rank == 1) :
			$rankstr = "st";
		elseif ($rank == 2) :
			$rankstr = "nd";
		elseif ($rank == 3) :
			$rankstr = "rd";
		else :
			$rankstr = "th";
		endif;

		if ($rank == 1) :
			$icon = "cupgoldg.png";
		elseif ($rank == 2) :
			$icon = "cupsilverg.png";
		elseif ($rank == 3) :
			$icon = "cupbronzeg.png";
		else :
			$icon = "";
		endif;

		echo "<img src='/db/ggzicons/games/$game.png' width=16 height=16>\n";
		echo "<a href='/db/games/?lookup=$game'>$game</a>:<br>\n";
		echo "$lookup ";
		if ($method == "wins/losses") :
			echo "achieved <b>$wins</b> wins, <b>$losses</b> losses, <b>$ties</b> ties, <b>$forfeits</b> forfeits.<br>\n";
		endif;
		if ($method == "rating") :
			echo "achieved a rating of <b>$rating</b>.<br>\n";
		endif;
		if ($method == "highscore") :
			echo "achieved a highscore of <b>$highscore</b>.<br>\n";
		endif;
		if ($icon) :
			echo "<img src='/db/ggzicons/rankings/$icon' title='Rank $rank' width='16' height='16'>\n";
		endif;
		echo "This is ranking him/her <b>$rank</b>$rankstr place.<br>\n";
		echo "<br>\n";
	}
	if (pg_numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	endif;
}

function stats_games($id, $lookup)
{
	global $ggzuser;

	if (!$lookup) return;

	$res = pg_exec($id, "SELECT * FROM rankings WHERE game = '$lookup'");
	if (($res) && (pg_numrows($res) == 1)) :
		$method = pg_result($res, 0, "method");
	else :
		$method = "wins/losses";
	endif;

	$res = pg_exec($id, "SELECT * FROM stats WHERE game = '$lookup' AND ranking > 0 ORDER BY ranking ASC");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$handle = pg_result($res, $i, "handle");
		$wins = pg_result($res, $i, "wins");
		$losses = pg_result($res, $i, "losses");
		$ties = pg_result($res, $i, "ties");
		$forfeits = pg_result($res, $i, "forfeits");
		$rating = pg_result($res, $i, "rating");
		$rank = pg_result($res, $i, "ranking");
		$highscore = pg_result($res, $i, "highscore");

		$rating = (int)($rating);

		$handlecaption = $handle;
		$pic = "player.png";
		if ($handle == $ggzuser) :
			$handlecaption = "<b>$handle</b>";
			$pic = "you.png";
		endif;

		if ($rank == 1) :
			$rankstr = "st";
		elseif ($rank == 2) :
			$rankstr = "nd";
		elseif ($rank == 3) :
			$rankstr = "rd";
		else :
			$rankstr = "th";
		endif;

		if ($rank == 1) :
			$icon = "cupgoldg.png";
		elseif ($rank == 2) :
			$icon = "cupsilverg.png";
		elseif ($rank == 3) :
			$icon = "cupbronzeg.png";
		else :
			$icon = "";
		endif;

		echo "<img src='/db/ggzicons/players/$pic' width=16 height=16>\n";
		echo "<a href='/db/players/?lookup=$handle'>$handlecaption</a>\n";
		if ($method == "wins/losses") :
			echo "achieved <b>$wins</b> wins, <b>$losses</b> losses, <b>$ties</b> ties, <b>$forfeits</b> forfeits.<br>\n";
		endif;
		if ($method == "rating") :
			echo "achieved a rating of <b>$rating</b>.<br>\n";
		endif;
		if ($method == "highscore") :
			echo "achieved a highscore of <b>$highscore</b>.<br>\n";
		endif;
		if ($icon) :
			echo "<img src='/db/ggzicons/rankings/$icon' title='Rank $rank' width='16' height='16'>\n";
		endif;
		echo "This is ranking him/her <b>$rank</b>$rankstr place.<br>\n";
		echo "<br>\n";
	}
}

function stats_team($id, $lookup)
{
	if (!$lookup) return;

	$res = pg_exec($id, "SELECT * FROM teams WHERE teamname = '$lookup'");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$fullname = pg_result($res, $i, "fullname");
		$icon = pg_result($res, $i, "icon");
		$foundingdate = pg_result($res, $i, "foundingdate");
		$founder = pg_result($res, $i, "founder");
		$homepage = pg_result($res, $i, "homepage");

		$date = date("d.m.Y", $foundingdate);

		echo "Team name: $fullname.<br>\n";
		echo "Founding date: $date.<br>\n";
		echo "Founded by: <a href='/db/players/?lookup=$founder'>$founder</a>.<br>\n";
		echo "Homepage: <a href='$homepage'>$homepage</a>.<br>\n";
		echo "Team logo:<br>\n";
		echo "<img src='$icon' height='64'>\n";
		echo "<br clear='all'>\n";
		echo "<br>\n";

	}
	if (pg_numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	else :
		echo "Members of the team:<br>\n";
		$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$lookup' AND role <> '' ORDER BY entrydate ASC");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$handle = pg_result($res, $i, "handle");
			$role = pg_result($res, $i, "role");
			$entrydate = pg_result($res, $i, "entrydate");

			$date = date("d.m.Y", $entrydate);

			$color = "silver";
			$number = 1;
			$attribute = "";
			$title = "";

			if (strstr($role, "founder")) :
				$title .= "Founder & ";
				$attribute = "s";
			endif;
			if (strstr($role, "leader")) :
				$title .= "Leader & ";
				$color = "gold";
				if ($attribute != "s") :
					$attribute = "d";
				endif;
			endif;
			if (strstr($role, "vice")) :
				$title .= "Vice Leader & ";
				$color = "gold";
			endif;

			$title = substr($title, 0, strlen($title) - 2);

			echo "<img src='/db/ggzicons/rankings/$color$attribute.png' title='$title'>\n";
			echo "<a href='/db/players/?lookup=$handle'>$handle</a> (since $date)<br>\n";
		}
	endif;
}

function stats_match($id, $lookup)
{
	if (!$lookup) return;

	$res = pg_exec($id, "SELECT * FROM matches WHERE id = '$lookup'");
	$game = pg_result($res, $i, "game");
	$date = pg_result($res, $i, "date");
	$winner = pg_result($res, $i, "winner");

	$date = date("d.m.Y", $date);

	echo "Match game type: $game<br>\n";
	echo "Date: $date<br>\n";
	echo "Winner: $winner<br>\n";
	echo "<br>\n";

	if (pg_numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	else :
		echo "Participants:<br>\n";
		$res = pg_exec($id, "SELECT * FROM matchplayers WHERE match = '$lookup'");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$handle = pg_result($res, $i, "handle");
			$playertype = pg_result($res, $i, "playertype");

			if ($playertype == "registered") :
				echo "<a href='/db/players/?lookup=$handle'>$handle ($playertype)</a><br>\n";
			else :
				echo "<span class='linklike'>$handle ($playertype)</span><br>\n";
			endif;
		}
	endif;
}

function stats_tournament($id, $lookup)
{
	if (!$lookup) return;

	echo "not yet!";
}

?>

<?php
include("statsclass.php");
?>

