<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Matches
		<span class="itemleader"> :: </span>
		<a name="match"></a>
	</h1>
	<h2>
		<span class="itemleader">:: </span>
		Game board result
		<span class="itemleader"> :: </span>
		<a name="matchboard"></a>
	</h2>
	<div class="text">
<?php

include_once("match.php");

$match = new Match($lookup);
$game = strtolower($match->game);

include_once("savegame_$game.php");

$savegamedir = $ggzgamedir;
$savegamefile = "$savegamedir/$match->game/$match->savegame";

$savegame = new Savegame();
$savegame->load($savegamefile);

echo "<img src='image_$game.php?savegamefile=$savegamefile'>\n";

?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Match history
		<span class="itemleader"> :: </span>
		<a name="matchhistory"></a>
	</h2>
	<div class="text">
<?php

$starttime = date("d.m.Y H:i:s", $savegame->starttime);
$endtime = date("d.m.Y H:i:s", $savegame->endtime);

if ($savegame->winner) :
	$winner = $match->link($savegame->winner);
else :
	$winner = "no winner (tie game)";
endif;

echo "Board size: $savegame->width x $savegame->height<br>\n";
echo "Started: $starttime<br>\n";
echo "Finished: $endtime<br>\n";
echo "Winner: $winner<br>\n";
echo "<br>\n";

echo $savegame->history;

?>
	</div>
</div>

<?php include("bottom.inc"); ?>
