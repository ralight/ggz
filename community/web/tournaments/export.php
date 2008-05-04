<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

header("Content-type: image/png");

include_once("match.php");

$res = $database->exec("SELECT * FROM tournaments WHERE id = '%^'", array($tid));

$name = $database->result($res, $i, "name");
$game = $database->result($res, $i, "game");
$room = $database->result($res, $i, "room");
$date = $database->result($res, $i, "date");
$organizer = $database->result($res, $i, "organizer");

$datestr = date("d.m.Y", $date);
$nowstr = date("d.m.Y", time());

$width = 1280;
$height = 800;

$im = imagecreatetruecolor($width, $height);

$fontfile = "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf";

$bgcolor = imagecolorallocate($im, 255, 255, 255);
$fgcolor = imagecolorallocate($im, 0, 0, 0);

imagefill($im, 0, 0, $bgcolor);

imagefttext($im, 14, 0, 20, 20, $fgcolor, $fontfile, "GGZ Gaming Zone site " . Config::getvalue("name"));
imagefttext($im, 12, 0, 20, 40, $fgcolor, $fontfile, "Tournament $name in room $room of type $game");
imagefttext($im, 12, 0, 20, 60, $fgcolor, $fontfile, "Start date: $datestr; current date: $nowstr");
imagefttext($im, 12, 0, 20, 80, $fgcolor, $fontfile, "Organizer: $organizer");

imagefttext($im, 12, 0, 20, 120, $fgcolor, $fontfile, "Participants:");

$res = $database->exec("SELECT * FROM tournamentplayers WHERE id = '%^'", array($tid));
$num = 0;
$match = new Match($tid);
$match->loadtournament();
for($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$number = $database->result($res, $i, "number");

	imagefttext($im, 10, 0, 30, 135 + $i * 15, $fgcolor, $fontfile, $name);

	$num++;
	$players[$number] = $name;
}

function played($player1, $player2)
{
	global $database;
	global $date;
	global $matchid;

	$res = $database->exec("SELECT match FROM matchplayers WHERE handle = '%^'", array($player1));
	for($k = 0; $k < $database->numrows($res); $k++)
	{
		$x[$database->result($res, $k, "match")] = $database->result($res, $k, "match");
	}

	$number = 0;
	$res = $database->exec("SELECT match FROM matchplayers WHERE handle = '%^'", array($player2));
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
		$res = $database->exec("SELECT id, winner FROM matches WHERE id = '%^' AND date > '%^'", array($match, $date));
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

$names = array("Finals", "Semi-finals", "Quarter-finals", "Qualification", "Pre-qualification");

$level = log($num) / log(2);

$open = false;
$op = 1;
$xnum = $num / 2;
$winners = array();
for($i = $level; $i > 0; $i--)
{
	$name = $names[$i - 1];

	$x = (3 - log(1)) * 160 + 100;

	imagefttext($im, 12, 0, $x, 110 + ($i - 1) * 100, $fgcolor, $fontfile, $name);

	if($i == $level) :
		for($j = 0; $j < $num; $j += 2)
		{
			$player1 = $players[$j];
			$player2 = $players[$j + 1];
			$winner = played($player1, $player2);
			if ($winner) :
				$winners[$op] = $winner;
				$winnerstr = $winner;
			else:
				$open = true;
			endif;

			$x = ($j + (3 - log($xnum))) * 80 + 50;

			imagefttext($im, 10, 0, $x, 135 + ($i - 1) * 100, $fgcolor, $fontfile, "$player1");
			imagefttext($im, 10, 0, $x, 150 + ($i - 1) * 100, $fgcolor, $fontfile, "$player2");
			if ($winner) :
				imagefttext($im, 10, 0, $x, 165 + ($i - 1) * 100, $fgcolor, $fontfile, "Winner: $winnerstr");
			endif;

			$op += 1;
		}
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
				$winnerstr = $winner;
			else:
				$open = true;
			endif;

			$x = ($j + (3 - log($xnum))) * 160 + 100;

			imagefttext($im, 10, 0, $x, 135 + ($i - 1) * 100, $fgcolor, $fontfile, "$player1");
			imagefttext($im, 10, 0, $x, 150 + ($i - 1) * 100, $fgcolor, $fontfile, "$player2");
			if ($winner) :
				imagefttext($im, 10, 0, $x, 165 + ($i - 1) * 100, $fgcolor, $fontfile, "Winner: $winnerstr");
			endif;

			$op1 += 2;
			$op2 += 2;
			$op += 1;
			$opoffset += 2;
		}
	endif;
}

imagepng($im);
imagedestroy($im);

?>
