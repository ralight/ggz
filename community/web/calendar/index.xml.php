<?php

if ($html) :
	echo "<html>\n";
	echo "<head>\n";
	echo "<title>\n";
	echo "GGZ Competition Calendar (XML output)\n";
	echo "</title>\n";
	echo "</head>\n";
	echo "<body>\n";

	echo "<pre>\n";
else :
	header("Content-type: text/plain");
endif;

include("database.php");

function output($x)
{
	global $html;

	if ($html) :
		echo htmlentities("$x");
		echo "<br>\n";
	else :
		echo "$x\n";
	endif;
}

output("<ggzcalendar>");

output("<series id=\"ggzcommunity\">");
output("  <organizer>GGZ Community</organizer>");
output("  <homepage>http://www.ggzcommunity.org/</homepage>");
output("  <registration>open</registration>");
output("  <title>GGZ Community Upcoming Tournaments</title>");
output("  <result>none</result>");
output("</series>");

output("<series id=\"ggzcommunity.done\">");
output("  <organizer>GGZ Community</organizer>");
output("  <homepage>http://www.ggzcommunity.org/</homepage>");
output("  <registration>open</registration>");
output("  <title>GGZ Community Finished Tournaments</title>");
output("  <result>none</result>");
output("</series>");

$res = pg_exec($id, "SELECT * FROM tournaments");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$gid = pg_result($res, $i, "id");
	$title = pg_result($res, $i, "name");
	$game = pg_result($res, $i, "game");
	$date = pg_result($res, $i, "date");
	$organizer = pg_result($res, $i, "organizer");

	$image = "";
	$status = "running";
	$policy = "open";

	output("<eventgroup id=\"$gid\" series=\"ggzcommunity\">");
	output("  <image>$image</image>");
	output("  <status>$status</status>");
	output("  <title>$title</title>");
	output("  <policy>$policy</policy>");
	output("  <game>$game</game>"); // this is new!
	output("</eventgroup>");

	$players = array();
	$res2 = pg_exec($id, "SELECT * FROM tournamentplayers WHERE id = $gid");
	for ($j = 0; $j < pg_numrows($res2); $j++)
	{
		$number = pg_result($res2, $j, "number");
		$name = pg_result($res2, $j, "name");

		$players[$number] = $name;
	}

	$names = array("Finals", "Semi-finals", "Quarter-finals", "Qualification", "Pre-qualification");
	$num = pg_numrows($res2);
	$xnum = $num;
	$level = log($num) / log(2);
	$offset = 0;

	for($j = $level; $j > 0; $j--)
	{
		$name = $names[$j - 1];

		for($k = 0; $k < $xnum; $k += 2)
		{
			if ($j == $level) :
				$player1 = $players[$k + $offset];
				$player2 = $players[$k + $offset + 1];
			else :
				$x = $k + $offset - $num;
				$y = $k + $offset - $num + 1;
				$player1 = "winner of ($x)";
				$player2 = "winner of ($y)";
			endif;

			$mid = $j;
			$pos = ($offset + $k) / 2;
			$title = "$game ($name/$pos)";
			if ($j == $level) :
				$description = "Initial game: $player1 vs. $player2";
			else :
				$prename = $names[$j];
				$description = "$name: depends on outcome of $prename ($player1 vs. $player2)";
			endif;
			$status = "";
			$image = ""; //"http://rankings.ggzgamingzone.org/ggzicons/games/$game.png";
			$location = "ggz://live.ggzgamingzone.org";

			$date += 1;

			output("<event id=\"$mid\" eventgroup=\"$gid\">");
			output("  <image>$image</image>");
			output("  <date>$date</date>");
			output("  <location>$location</location>");
			output("  <title>$title</title>");
			output("  <description>$description</description>");
			output("  <status>$status</status>");
			output("  <participants>");
			output("   <participant team='$player1'>$player1</participant>");
			output("   <participant team='$player2'>$player2</participant>");
			output("  </participants>");
			output("  <scores>");
			output("  </scores>");
			output("</event>");
		}

		$offset += $xnum;
		$xnum = $xnum / 2;
	}
}

output("</ggzcalendar>");

if ($html) :
	echo "</pre>\n";

	echo "</body>\n";
	echo "</html>\n";
endif;

?>

