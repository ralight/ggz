<?php

function rankings_players($id, $lookup)
{
	echo "<b>Rankings</b><br>\n";

	$res = pg_exec($id, "SELECT * FROM stats WHERE handle = '$lookup' AND ranking > 0 AND ranking < 4");

	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$rank = pg_result($res, $i, "ranking");
		$game = pg_result($res, $i, "game");

		if ($rank == 1) :
			$icon = "cupgoldg.png";
		elseif ($rank == 2) :
			$icon = "cupsilverg.png";
		else :
			$icon = "cupbronzeg.png";
		endif;

		echo "<img src='/db/ggzicons/rankings/$icon' title='Rank $rank'>\n";
		echo "Game class $game: Rank $rank\n";
		echo "<br>\n";
	}

	/*
	echo "Global:\n";
	echo "<img src='/db/ggzicons/rankings/cupgolda.png' title='Rank 1'>\n";
	echo "<img src='/db/ggzicons/rankings/cupsilvera.png' title='Rank 2'>\n";
	echo "<img src='/db/ggzicons/rankings/cupbronzea.png' title='Rank 3'>\n";
	echo "<br>\n";
	echo "Tournament class Bar:\n";
	echo "<img src='/db/ggzicons/rankings/cupsilvert.png' title='Rank 2'>\n";
	echo "<br>\n";
	*/

	echo "<br><br>\n";

	echo "<b>Winner Titles (tournaments)</b><br>\n";

	$res = pg_exec($id, "SELECT * FROM placements WHERE handle = '$lookup'");

	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$tournament = pg_result($res, $i, "tournament");
		$place = pg_result($res, $i, "place");

		if ($place == 1) :
			$ranking = "1st place";
			$icon = "cupgold.png";
		else :
			$ranking = "2nd place";
			$icon = "cupsilver.png";
		endif;

		$res2 = pg_exec($id, "SELECT * FROM tournaments WHERE id = $tournament");
		$game = pg_result($res2, 0, "game");
		$name = pg_result($res2, 0, "name");
		$stamp = pg_result($res2, 0, "date");

		$date = date("d.m.Y", $stamp);


		echo "<img src='/db/ggzicons/rankings/$icon' title='$ranking'>\n";
		echo "Tournament '$name' of gametype $game ($date): $ranking\n";
		echo "<br>\n";
	}
	if(!pg_numrows($res))
	{
		echo "You haven't won a single tournament yet.\n";
	}

	echo "<br><br>\n";

	echo "<b>Winner Titles (individual games)</b><br>\n";

	$res = pg_exec($id, "SELECT * FROM matches WHERE winner = '$lookup'");

	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$game = pg_result($res, $i, "game");
		$stamp = pg_result($res, $i, "date");

		$date = date("d.m.Y", $stamp);

		echo "<img src='/db/ggzicons/rankings/coingold.png' title='Winner'>\n";
		echo "Game of gametype $game ($date): Winner\n";
		echo "<br>\n";
	}
	if(!pg_numrows($res))
	{
		echo "You haven't won a single game yet.\n";
	}
}

?>

