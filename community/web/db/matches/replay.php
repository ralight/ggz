<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Matches
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php

$savegamedir = "/home/ggz/BUILD/var/ggzd/gamedata/";
$savegame = "$savegamedir/TicTacToe/savegame.NFZl2v";

include("savegame.php");

$game = new Savegame();
$game->load($savegame);

echo "Board size: $game->width x $game->height<br>\n";
echo "Started: $game->starttime<br>\n";
echo "Finished: $game->endtime<br>\n";
echo "Winner: $game->winner<br>\n";
echo "<br>\n";

echo "<img src='image.php?savegame=$savegame'>\n";

?>
	</div>
</div>

<?php include("bottom.inc"); ?>
