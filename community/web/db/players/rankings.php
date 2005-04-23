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
		else :
			$ranking = "2nd place";
			$icon = "cupsilver.png";
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

?>

