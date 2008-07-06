<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

//$format = $_GET["format"];

if ($format == "svg") :
	header("Content-type: image/svg+xml");
else :
	header("Content-type: image/png");
	$format = "png";
endif;

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

if ($format == "png") :
	$im = imagecreatetruecolor($width, $height);

	$fontfile = "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf";
	$fontfileit = "/usr/share/fonts/truetype/ttf-bitstream-vera/VeraIt.ttf";

	$bgcolor = imagecolorallocate($im, 255, 255, 255);
	$fgcolor = imagecolorallocate($im, 0, 0, 0);

	imagefill($im, 0, 0, $bgcolor);

	imagefttext($im, 14, 0, 20, 20, $fgcolor, $fontfile, "GGZ Gaming Zone site " . Config::getvalue("name"));
	imagefttext($im, 12, 0, 20, 40, $fgcolor, $fontfile, "Tournament $name in room $room of type $game");
	imagefttext($im, 12, 0, 20, 60, $fgcolor, $fontfile, "Start date: $datestr; current date: $nowstr");
	imagefttext($im, 12, 0, 20, 80, $fgcolor, $fontfile, "Organizer: $organizer");

	imagefttext($im, 12, 0, 20, 120, $fgcolor, $fontfile, "Participants:");
elseif ($format == "svg") :
	echo "<svg xmlns:svg='http://www.w3.org/2000/svg' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='$width' height='$height'>\n";
endif;

$res = $database->exec("SELECT * FROM tournamentplayers WHERE id = '%^'", array($tid));
$num = 0;
$match = new Match($tid);
$match->loadtournament();
for($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$number = $database->result($res, $i, "number");

	if ($format == "png") :
		imagefttext($im, 10, 0, 30, 135 + $i * 15, $fgcolor, $fontfileit, $name);
	endif;

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

function drawsvgbox($xb, $yb, $winner, $tid, $player1, $player2)
{
	$color = "ffffff";
	if ($winner) :
		$color = "e1e956";
		echo "<a xlink:href='/db/tournaments/?lookup=$tid' xlink:title='Match details'>\n";
	endif;
	echo "<rect width='88' height='62' x='" . $xb . "' y='" . $yb . "' style='fill:#$color;stroke:#000000'/>\n";
	if ($winner) :
		echo "</a>\n";
	endif;
	echo "<rect width='44' height='51' x='" . $xb . "' y='" . ($yb + 62) . "' style='fill:#e1e9b4;stroke:#000000'/>\n";
	echo "<rect width='44' height='51' x='" . ($xb + 44) . "' y='" . ($yb + 62) . "' style='fill:#e1e9b4;stroke:#000000'/>\n";
	$textstyle="font-size:7px;fill:#000000;stroke:none;font-family:Bitstream Vera Serif";
	if ($player1) :
		// $p = new Player($player1);
		// if ($p->photo) : ...
		echo "<a xlink:href='/db/players/?lookup=$player1' xlink:title='Player page'>\n";
		echo "<image width='34' height='31' x='" . ($xb + 5) . "' y='" . ($yb + 65) . "' xlink:href='/images/akademy08/pagelogo.png'/>\n";
		echo "</a>\n";
		echo "<text style='$textstyle'>\n";
		echo "<tspan x='" . ($xb + 5) . "' y='" . ($yb + 106) . "'>\n";
		echo $player1;
		if ($winner == $player1) :
			echo " *";
		endif;
		echo "</tspan>\n";
		echo "</text>\n";
	endif;
	if ($player2) :
		echo "<a xlink:href='/db/players/?lookup=$player2' xlink:title='Player page'>\n";
		echo "<image width='34' height='31' x='" . ($xb + 49) . "' y='" . ($yb + 65) . "' xlink:href='/images/akademy08/pagelogo.png'/>\n";
		echo "</a>\n";
		echo "<text style='$textstyle'>\n";
		echo "<tspan x='" . ($xb + 49) . "' y='" . ($yb + 106) . "'>\n";
		echo $player2;
		if ($winner == $player2) :
			echo " *";
		endif;
		echo "</tspan>\n";
		echo "</text>\n";
	endif;
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

	if ($format == "png") :
		imagefttext($im, 12, 0, $x, 110 + ($i - 1) * 100, $fgcolor, $fontfile, $name);
	endif;

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

			$x = (($width - 100) / 2) + $j * 80 - ($xnum - 1) * 80;

			if ($format == "png") :
				imagefttext($im, 10, 0, $x, 135 + ($i - 1) * 100, $fgcolor, $fontfile, "Match: $op");
				imagefttext($im, 10, 0, $x, 150 + ($i - 1) * 100, $fgcolor, $fontfileit, "$player1");
				imagefttext($im, 10, 0, $x, 165 + ($i - 1) * 100, $fgcolor, $fontfileit, "$player2");
				if ($winner) :
					imagefttext($im, 10, 0, $x, 180 + ($i - 1) * 100, $fgcolor, $fontfile, "Winner: $winnerstr");
				endif;
			elseif ($format == "svg") :
				$xb = $x;
				$yb = $height - $i * 150;
				drawsvgbox($xb, $yb, $winner, $tid, $player1, $player2);
			endif;

			$op += 1;
		}
	else :
		$xnum = $xnum / 2;
		$op1 = 1 + $opoffset;
		$op2 = 2 + $opoffset;
		for($j = 0; $j < $xnum; $j++)
		{
			$fontfilep1 = $fontfile;
			$fontfilep2 = $fontfile;

			$player1 = "winner of ($op1)";
			$player2 = "winner of ($op2)";
			if ($winners[$op1]) :
				$player1 = $winners[$op1];
				$fontfilep1 = $fontfileit;
			endif;
			if ($winners[$op2]) :
				$player2 = $winners[$op2];
				$fontfilep2 = $fontfileit;
			endif;
			$winner = played($player1, $player2);
			if ($winner) :
				$winners[$op] = $winner;
				$winnerstr = $winner;
			else:
				$open = true;
			endif;

			$x = (($width - 100) / 2) + $j * 160 - ($xnum - 1) * 80;

			if ($format == "png") :
				imagefttext($im, 10, 0, $x, 135 + ($i - 1) * 100, $fgcolor, $fontfile, "Match: $op");
				imagefttext($im, 10, 0, $x, 150 + ($i - 1) * 100, $fgcolor, $fontfilep1, "$player1");
				imagefttext($im, 10, 0, $x, 165 + ($i - 1) * 100, $fgcolor, $fontfilep2, "$player2");
				if ($winner) :
					imagefttext($im, 10, 0, $x, 180 + ($i - 1) * 100, $fgcolor, $fontfile, "Winner: $winnerstr");
				endif;
			elseif ($format == "svg") :
				$xb = $x;
				$yb = $height - $i * 150;
				drawsvgbox($xb, $yb, $winner, $tid, $winners[$op1], $winners[$op2]);

				$jleft = $j * 2;
				$jright = $j * 2 + 1;

				$xold = (($width - 100) / 2) + $jleft * 160 - ($xnum * 2 - 1) * 80;
				$xold2 = (($width - 100) / 2) + $jright * 160 - ($xnum * 2 - 1) * 80;
				$xold += 44;
				$xold2 += 44;
				$yold = $yb - 150 + 113;
				$xbc = $xb + 44;
				echo "<path style='fill:none;fill-rule:evenodd;stroke:#000000' d='M $xbc,$yb L $xold,$yold'/>\n";
				echo "<path style='fill:none;fill-rule:evenodd;stroke:#000000' d='M $xbc,$yb L $xold2,$yold'/>\n";
			endif;

			$op1 += 2;
			$op2 += 2;
			$op += 1;
			$opoffset += 2;
		}
	endif;
}

if ($format == "png") :
	imagepng($im);
	imagedestroy($im);
elseif ($format == "svg") :
	echo "</svg>\n";
endif;

?>
