<?php include("head.inc"); ?>

<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<b>Overview</b>
<br><br>

<?php

$conn = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

$res = pg_exec($conn, "SELECT * FROM tournaments WHERE id = $id");

$name = pg_result($res, $i, "name");
$game = pg_result($res, $i, "game");
$date = pg_result($res, $i, "date");

$datestr = date("d.m.Y", $date);

echo "Selected tournament: $name ($game)<br>\n";
echo "Starting: $datestr<br><br>\n";

?>

</td></tr></table>
</td></tr>
<tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#a0ff00'>

<?php

echo "Participants:<br><br>\n";
$res = pg_exec($conn, "SELECT * FROM tournamentplayers WHERE id = $id");
$num = 0;
for($i = 0; $i < pg_numrows($res); $i++)
{
	$name = pg_result($res, $i, "name");
	$number = pg_result($res, $i, "number");

	echo "$name<br>\n";

	$num++;
	$players[$number] = $name;
}

echo "<br>\n";

?>

</td></tr></table>
</td></tr>
<tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#f05f00'>

<?php

function played($player1, $player2)
{
	global $conn;
	global $date;

	$res = pg_exec($conn, "SELECT match FROM matchplayers WHERE handle = '$player1'");
	for($k = 0; $k < pg_numrows($res); $k++)
	{
		$x[pg_result($res, $k, "match")] = pg_result($res, $k, "match");
	}

	$number = 0;
	$res = pg_exec($conn, "SELECT match FROM matchplayers WHERE handle = '$player1'");
	for($k = 0; $k < pg_numrows($res); $k++)
	{
		if($x[pg_result($res, $k, "match")] == pg_result($res, $k, "match")) :
			$y[$number] = $x[pg_result($res, $k, "match")];
			$number += 1;
		endif;
	}

	for($k = 0; $k < sizeof($y); $k++)
	{
		$match = $y[$k];
		$res = pg_exec($conn, "SELECT winner FROM matches WHERE id = $match AND date > $date");
		if(pg_numrows($res) == 1) :
			$winner = pg_result($res, 0, "winner");
			if($winner) :
				return $winner;
			endif;
		endif;
	}

	return "";
}

echo "Tournament plan:<br><br>\n";

$names = array("Finals", "Semi-finals", "Quarter-finals", "Qualification", "Pre-qualification");

$level = log($num) / log(2); // 2^$level=$num -> $level=ld($num)=log($num)/log(2)

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
			else:
				$winnerstr = "<b>(open)</b>";
				$open = true;
			endif;
			echo "($op) $player1 vs. $player2: $winnerstr<br>\n";

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
			else:
				$winnerstr = "<b>(open)</b>";
				$open = true;
			endif;
			echo "($op) $player1 vs. $player2: $winnerstr<br>\n";
			$op1 += 2;
			$op2 += 2;
			$op += 1;
			$opoffset += 2;
		}
		echo "<br>\n";
	endif;
}

$res = pg_exec($conn, "SELECT * FROM placements WHERE tournament = $id");
if (pg_numrows($res) > 0) :
	$already_approved = 1;
else :
	$already_approved = 0;
endif;

if (($approved) && (!$already_approved) && (!$open)) :
	if ($player1 == $winner) :
		$second = $player2;
	else:
		$second = $player1;
	endif;
	pg_exec($conn, "INSERT INTO placements " .
		"(tournament, place, handle) VALUES " .
		"($id, 2, '$second')");
	pg_exec($conn, "INSERT INTO placements " .
		"(tournament, place, handle) VALUES " .
		"($id, 1, '$winner')");
endif;

?>

<br><br>

</td></tr></table>
</td></tr>
<tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#e07f00'>

<?php

if (!$open) :

if ($already_approved) :
	echo "This result has been approved.\n";
else:
	echo "<a href='show2.php?id=$id&approved=1'>Approve this result!</a>\n";
endif;

endif;

?>

</td></tr></table>
</td></tr>
<tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<a href="index.php">Back to index</a>

<br><br>

</td></tr></table>
</td></tr></table>

<?php include("foot.inc"); ?>
