	<h2>
		<span class="itemleader">:: </span>
		Public information
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php

include_once("player.php");

$player = new Player($lookup);
$player->display();

?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Statistics by game
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
	<?php stats_players($id, $lookup); ?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Tournaments
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include_once("auth.php");
$ggzuser = $lookup;
$res = pg_exec($id, "SELECT * FROM tournaments WHERE organizer = '$ggzuser'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$name = pg_result($res, $i, "name");
	$tid = pg_result($res, $i, "id");
	echo "<a href='/db/tournaments/?lookup=$tid'>$name</a><br>\n";
}
?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Teams
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
//include_once("auth.php");
//$ggzuser = Auth::username();
$res = pg_exec($id, "SELECT * FROM teammembers WHERE handle = '$ggzuser'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$teamname = pg_result($res, $i, "teamname");
	$role = pg_result($res, $i, "role");
	echo "<a href='/db/teams/?lookup=$teamname'>$teamname</a> ($role)<br>\n";
}
?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Rankings
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include("rankings.php");
rankings_players($id, $lookup);
?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Winner Titles (tournaments)
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
rankings_tournaments($id, $lookup);
?>
	</div>
	<h2>
		<span class="itemleader">:: </span>
		Winner Titles (individual games)
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
rankings_matches($id, $lookup);
?>
	</div>
