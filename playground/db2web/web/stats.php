<?php

function stats_players($id, $lookup)
{
	$res = pg_exec($id, "SELECT * FROM stats WHERE lower(handle) = '$lookup'");
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

	$res = pg_exec($id, "SELECT COUNT(*) FROM tournaments");
	$count = pg_result($res, 0, "count");
	echo "<b>$count</b> tournaments have been organized.<br>\n";

	$res = pg_exec($id, "SELECT COUNT(*) FROM matches");
	$count = pg_result($res, 0, "count");
	echo "<b>$count</b> individual games have been played.<br>\n";
}

function stats_live($host)
{

	$f = @fsockopen($host, "8080", $errno, $errstr, 10);
	if ($f) :
		echo "The following people are currently playing games on $host:<br>\n";

		do
		{
			$tmp = fgets($f, 1024);
			$result .= $tmp;
		}
		while ($tmp);
		fclose($f);

		$xmlparser = xml_parser_create();
		xml_set_element_handler($xmlparser, "xmlStart", "xmlEnd");
		xml_set_character_data_handler($xmlparser, "xmlData");
		xml_parse($xmlparser, $result);
		xml_parser_free($xmlparser);
	else :
		echo "List of running games could not be determined.<br>\n";
	endif;
}

function xmlStart($parser, $name, $attributes)
{
	global $context;

	$context = $name;

	if ($context == "GAME") :
		echo "Game of ", $attributes['NAME'], ":<br>\n";
	elseif ($context == "TABLE") :
		echo "* Table ", $attributes['ID'], ":<br>\n";
	endif;
}

function xmlEnd($parser, $name)
{
	global $context;

	$context = "";
}

function xmlData($parser, $data)
{
	global $context;

	if ($context == "SEAT") :
		echo "&nbsp;&nbsp;&nbsp;";
		echo "Seat: $data<br>\n";
	endif;
}

function stats_team($id, $lookup)
{
	$res = pg_exec($id, "SELECT * FROM teams WHERE teamname = '$lookup'");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$fullname = pg_result($res, $i, "fullname");
		$icon = pg_result($res, $i, "icon");
		$foundingdate = pg_result($res, $i, "foundingdate");
		$homepage = pg_result($res, $i, "homepage");

		$date = date("d.m.Y", $foundingdate);

		echo "Team name: $fullname.<br>\n";
		echo "Founding date: $date.<br>\n";
		echo "Homepage: <a href='$homepage'>$homepage</a>.<br>\n";
		echo "<br>\n";

	}
	if (pg_numrows($res) == 0) :
		echo "No statistics found for $lookup.<br>\n";
	else :
		echo "Members of the team:<br>\n";
		$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$lookup'");
		for ($i = 0; $i < pg_numrows($res); $i++)
		{
			$handle = pg_result($res, $i, "handle");
			$role = pg_result($res, $i, "role");

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

			echo "<img src='ggzicons/rankings/$color$attribute.png' title='$title'>\n";
			echo "<a href='index.php?lookup=$handle&amp;type=player'>$handle</a><br>\n";
		}
	endif;
}

?>

