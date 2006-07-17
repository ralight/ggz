<?php

function rankings_players($lookup)
{
	global $database;

	$res = $database->exec("SELECT * FROM stats WHERE handle = '$lookup' AND ranking > 0 AND ranking < 4");

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$rank = $database->result($res, $i, "ranking");
		$game = $database->result($res, $i, "game");

		if ($rank == 1) :
			$icon = "cupgoldg.png";
		elseif ($rank == 2) :
			$icon = "cupsilverg.png";
		else :
			$icon = "cupbronzeg.png";
		endif;

		$gamelink = "<a href='/db/games/?lookup=$game'>$game</a>";

		echo "<img src='/db/ggzicons/rankings/$icon' title='Rank $rank'>\n";
		echo "Game class $gamelink: Rank $rank\n";
		echo "<br>\n";
	}
	if(!$database->numrows($res))
	{
		echo "The player has not yet played any games.\n";
	}
}

function rankings_tournaments($lookup)
{
	global $database;

	$res = $database->exec("SELECT * FROM placements WHERE handle = '$lookup'");

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$tournament = $database->result($res, $i, "tournament");
		$place = $database->result($res, $i, "place");

		if ($place == 1) :
			$ranking = "1st place";
			$icon = "cupgold.png";
		elseif ($place == 2) :
			$ranking = "2nd place";
			$icon = "cupsilver.png";
		elseif ($place == 3) :
			$ranking = "3rd place";
			$icon = "cupbronze.png";
		else :
			continue;
		endif;

		$res2 = $database->exec("SELECT * FROM tournaments WHERE id = $tournament");
		$game = $database->result($res2, 0, "game");
		$name = $database->result($res2, 0, "name");
		$stamp = $database->result($res2, 0, "date");

		$date = date("d.m.Y", $stamp);

		$tournamentlink = "<a href='/db/tournaments/?lookup=$tournament'>$name</a>";
		$gamelink = "<a href='/db/games/?lookup=$game'>$game</a>";

		echo "<img src='/db/ggzicons/rankings/$icon' title='$ranking'>\n";
		echo "Tournament $tournamentlink of gametype $gamelink ($date): $ranking\n";
		echo "<br>\n";
	}
	if(!$database->numrows($res))
	{
		echo "The player hasn't won a single tournament yet.\n";
	}
}

function rankings_matches($lookup)
{
	global $database;

	$res = $database->exec("SELECT * FROM matches WHERE winner = '$lookup'");

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$id = $database->result($res, $i, "id");
		$game = $database->result($res, $i, "game");
		$stamp = $database->result($res, $i, "date");

		$date = date("d.m.Y", $stamp);

		$matchlink = "<a href='/db/matches/?lookup=$id'>Match</a>";
		$gamelink = "<a href='/db/games/?lookup=$game'>$game</a>";

		echo "<img src='/db/ggzicons/rankings/coingold.png' title='Winner'>\n";
		echo "$matchlink of gametype $gamelink ($date): Winner\n";
		echo "<br>\n";
	}
	if(!$database->numrows($res))
	{
		echo "The player hasn't won a single game yet.\n";
	}
}

function rankings_matches_summary($lookup)
{
	global $database;

	$res = $database->exec("SELECT COUNT(*) FROM matches WHERE winner = '$lookup'");

	if ($database->numrows($res) == 1) :
		$count = $database->result($res, 0, "count");

		if ($count > 0) :
			$winnerpic = "coin-10";
			if ($count > 100) :
				$winnerpic = "coin-max";
			elseif ($count > 50) :
				$winnerpic = "coin-100";
			elseif ($count > 10) :
				$winnerpic = "coin-50";
			endif;

			echo "<img src='/db/ggzicons/rankings/$winnerpic.png' title='Winner'>\n";
			echo "This player has won $count matches of various games.\n";
			echo "The <a href='titles.php?lookup=$lookup'>match results page</a>\n";
			echo "contains the details.\n";
			echo "<br>\n";

			return;
		endif;
	endif;

	echo "The player hasn't won a single game yet.\n";
}

?>

