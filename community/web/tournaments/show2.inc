<?php

$res = $database->exec("SELECT * FROM tournaments WHERE id = $tid");

$name = $database->result($res, $i, "name");
$game = $database->result($res, $i, "game");
$date = $database->result($res, $i, "date");
$organizer = $database->result($res, $i, "organizer");

$datestr = date("d.m.Y", $date);

echo "Selected tournament: $name<br>\n";
echo "Game type: <a href=\"/db/games/?lookup=$game\">$game</a><br>\n";
echo "Organized by: <a href=\"/db/players/?lookup=$organizer\">$organizer</a><br>\n";
echo "Starting: $datestr<br><br>\n";

echo "Participants:<br><br>\n";
$res = $database->exec("SELECT * FROM tournamentplayers WHERE id = $tid");
$num = 0;
for($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$number = $database->result($res, $i, "number");

	echo "<a href=\"/db/players/?lookup=$name\">$name</a><br>\n";

	$num++;
	$players[$number] = $name;
}

echo "<br>\n";

function played($player1, $player2)
{
	global $database;
	global $date;
	global $matchid;

	$res = $database->exec("SELECT match FROM matchplayers WHERE handle = '$player1'");
	for($k = 0; $k < $database->numrows($res); $k++)
	{
		$x[$database->result($res, $k, "match")] = $database->result($res, $k, "match");
	}

	$number = 0;
	$res = $database->exec("SELECT match FROM matchplayers WHERE handle = '$player2'");
	for($k = 0; $k < $database->numrows($res); $k++)
	{
		if($x[$database->result($res, $k, "match")] == $database->result($res, $k, "match")) :
			$y[$number] = $x[$database->result($res, $k, "match")];
			$number += 1;
		endif;
	}

	for($k = 0; $k < sizeof($y); $k++)
	{
		$match = $y[$k];
		$res = $database->exec("SELECT id, winner FROM matches WHERE id = $match AND date > $date");
		if($database->numrows($res) == 1) :
			$winner = $database->result($res, 0, "winner");
			$matchid = $database->result($res, 0, "id");
			if($winner) :
				return $winner;
			endif;
		endif;
	}

	return "";
}

echo "Tournament plan:<br><br>\n";

$names = array("Finals", "Semi-finals", "Quarter-finals", "Qualification", "Pre-qualification");

$level = log($num) / log(2);

$open = false;
$op = 1;
$xnum = $num / 2;
$winners = array();
for($i = $level; $i > 0; $i--)
{
	$name = $names[$i - 1];
	echo "$name<br>\n";

	if($i == $level) :
		for($j = 0; $j < $num; $j += 2)
		{
			$player1 = $players[$j];
			$player2 = $players[$j + 1];
			$winner = played($player1, $player2);
			if ($winner) :
				$winners[$op] = $winner;
				$winnerstr = "<b>$winner</b>";
				$matchstr = " (<a href='/db/matches/?lookup=$matchid'>match details</a>)";
			else:
				$winnerstr = "<b>(open)</b>";
				$matchstr = "";
				$open = true;
			endif;
			echo "($op) $player1 vs. $player2: $winnerstr$matchstr<br>\n";

			$op += 1;
		}
		echo "<br>\n";
	else :
		$xnum = $xnum / 2;
		$op1 = 1 + $opoffset;
		$op2 = 2 + $opoffset;
		for($j = 0; $j < $xnum; $j++)
		{
			$player1 = "winner of ($op1)";
			$player2 = "winner of ($op2)";
			if ($winners[$op1]) :
				$player1 = $winners[$op1];
			endif;
			if ($winners[$op2]) :
				$player2 = $winners[$op2];
			endif;
			$winner = played($player1, $player2);
			if ($winner) :
				$winners[$op] = $winner;
				$winnerstr = "<b>$winner</b>";
				$matchstr = " (<a href='/db/matches/?lookup=$matchid'>match details</a>)";
			else:
				$winnerstr = "<b>(open)</b>";
				$matchstr = "";
				$open = true;
			endif;
			echo "($op) $player1 vs. $player2: $winnerstr$matchstr<br>\n";
			$op1 += 2;
			$op2 += 2;
			$op += 1;
			$opoffset += 2;
		}
		echo "<br>\n";
	endif;
}

$res = $database->exec("SELECT * FROM placements WHERE tournament = $tid");
if ($database->numrows($res) > 0) :
	$already_approved = 1;
else :
	$already_approved = 0;
endif;

if (Auth::username() != $organizer) :
	$approved = 0;
endif;

if (($approved) && (!$already_approved) && (!$open)) :
	if ($player1 == $winner) :
		$second = $player2;
	else:
		$second = $player1;
	endif;
	$database->exec("INSERT INTO placements " .
		"(tournament, place, handle) VALUES " .
		"($tid, 2, '$second')");
	$database->exec("INSERT INTO placements " .
		"(tournament, place, handle) VALUES " .
		"($tid, 1, '$winner')");
endif;

if (!$open) :

if ($already_approved) :
	echo "This result has been approved.\n";
else:
	if (Auth::username() == $organizer) :
		echo "<a href='index.php?page=show2&tid=$tid&approved=1'>Approve this result!</a>\n";
	else :
		echo "$Waiting for approval by tournament organizer.\n";
	endif;
endif;

else :

echo "This tournament is still running!\n";

endif;

?>

