<?php

include_once("player.php");
include_once("match.php");

function stats_players($lookup)
{
global $database;
	if (!$lookup) return;

	$player = new Player($lookup);

	$res = $database->exec("SELECT * FROM stats WHERE lower(handle) = lower('$lookup') AND ranking > 0");
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$game = $database->result($res, $i, "game");
		$wins = $database->result($res, $i, "wins");
		$losses = $database->result($res, $i, "losses");
		$ties = $database->result($res, $i, "ties");
		$forfeits = $database->result($res, $i, "forfeits");
		$rating = $database->result($res, $i, "rating");
		$rank = $database->result($res, $i, "ranking");
		$highscore = $database->result($res, $i, "highscore");

		$rating = (int)($rating);

		$res2 = $database->exec("SELECT * FROM rankings WHERE game = '$game'");
		if (($res2) && ($database->numrows($res2) == 1)) :
			$method = $database->result($res2, 0, "method");
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

		$genderref = $player->genderref();

		echo "This is ranking $genderref <b>$rank</b>$rankstr place.<br>\n";
		echo "<br>\n";
	}
	if ($database->numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	endif;
}

function stats_gamesheader($lookup)
{
	$game = $lookup;

	echo "<img src='/db/ggzicons/games/$game.png' width=16 height=16>\n";
	echo $lookup;
}

function stats_matchheader($lookup)
{
	echo "Match #$lookup";
}

function stats_tournamentheader($lookup)
{
	echo "Tournament #$lookup";
}

function stats_teamheader($lookup)
{
	echo "Team $lookup";
}

function stats_games($lookup)
{
global $database;
	global $ggzuser;

	if (!$lookup) return;

	$res = $database->exec("SELECT * FROM rankings WHERE game = '$lookup'");
	if (($res) && ($database->numrows($res) == 1)) :
		$method = $database->result($res, 0, "method");
	else :
		$method = "wins/losses";
	endif;

	$res = $database->exec("SELECT * FROM stats WHERE game = '$lookup' AND ranking > 0 ORDER BY ranking ASC");
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$wins = $database->result($res, $i, "wins");
		$losses = $database->result($res, $i, "losses");
		$ties = $database->result($res, $i, "ties");
		$forfeits = $database->result($res, $i, "forfeits");
		$rating = $database->result($res, $i, "rating");
		$rank = $database->result($res, $i, "ranking");
		$highscore = $database->result($res, $i, "highscore");

		$rating = (int)($rating);

		$handlecaption = $handle;

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

		$player = new Player($handle);
		$player->icon();

		if ($player->registered) :
			echo "<a href='/db/players/?lookup=$handle'>$handlecaption</a>\n";
		else :
			echo "<span class='linklike'>$handlecaption</span>\n";
		endif;

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

		$genderref = $player->genderref();

		echo "This is ranking $genderref <b>$rank</b>$rankstr place.<br>\n";
		echo "<br>\n";
	}
}

function stats_team($lookup)
{
global $database;
	if (!$lookup) return;

	$res = $database->exec("SELECT * FROM teams WHERE teamname = '$lookup'");
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$fullname = $database->result($res, $i, "fullname");
		$icon = $database->result($res, $i, "icon");
		$foundingdate = $database->result($res, $i, "foundingdate");
		$founder = $database->result($res, $i, "founder");
		$homepage = $database->result($res, $i, "homepage");

		$date = date("d.m.Y", $foundingdate);

		echo "Team name: $fullname.<br>\n";
		echo "Founding date: $date.<br>\n";
		echo "Founded by: <a href='/db/players/?lookup=$founder'>$founder</a>.<br>\n";
		echo "Homepage: <a href='$homepage'>$homepage</a>.<br>\n";
		if ($icon) :
			echo "Team logo:<br>\n";
			echo "<img src='$icon' height='64'>\n";
			echo "<br clear='all'>\n";
		endif;
		echo "<br>\n";

	}
	if ($database->numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	else :
		echo "Members of the team:<br>\n";
		$res = $database->exec("SELECT * FROM teammembers WHERE teamname = '$lookup' AND role <> '' ORDER BY entrydate ASC");
		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$handle = $database->result($res, $i, "handle");
			$role = $database->result($res, $i, "role");
			$entrydate = $database->result($res, $i, "entrydate");

			$date = date("d.m.Y", $entrydate);

			$color = "silver";
			$number = 1;
			$attribute = "";
			$title = "";

			if (strstr($role, "founder")) :
				$title .= "Founder";
				$attribute = "s";
			endif;
			if (strstr($role, "leader")) :
				if ($title) $title .= " &amp; ";
				$title .= "Leader";
				$color = "gold";
				if ($attribute != "s") :
					$attribute = "d";
				endif;
			endif;
			if (strstr($role, "vice")) :
				if ($title) $title .= " &amp; ";
				$title .= "Vice Leader";
				$color = "gold";
			endif;
			if (!$title) :
				$title = "Member";
			endif;

			echo "<img src='/db/ggzicons/rankings/$color$attribute.png' title='$title'>\n";
			echo "<a href='/db/players/?lookup=$handle'>$handle</a> (since $date)<br>\n";
		}
	endif;
}

function stats_match($lookup)
{
	if (!$lookup) return;

	$match = new Match($lookup);
	if(!$match->game) :
		echo "No statistics found for $lookup.<br>\n";
		return;
	endif;

	echo "Match game type: $match->game<br>\n";
	echo "Date: $match->date<br>\n";
	echo "Winner: $match->winner<br>\n";
	echo "<br>\n";

	echo "Participants:<br>\n";
	foreach ($match->playertypes as $handle => $playertype)
	{
		echo $match->link($handle), "<br>";
	}

	if ($match->savegame) :
		echo "<br>";
		echo "A savegame for this match is available.<br>";
		echo "<a href='replay.php?lookup=$lookup'>Examine the match</a>";
	endif;
}

function stats_tournament($lookup)
{
global $database;
global $date;
	if (!$lookup) return;

	$tid = $lookup;

	include("../../tournaments/show2.inc");
}

?>

<?php
include("statsclass.php");
?>

