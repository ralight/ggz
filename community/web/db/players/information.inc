<?php
include_once("player.php");
$player = new Player($lookup);
?>
	<h2>
		<span class="itemleader">:: </span>
		<?php $player->icon(); ?> Public information for player <?php echo $lookup; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
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
	<?php stats_players($lookup); ?>
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
$res = $database->exec("SELECT * FROM tournaments WHERE organizer = '$ggzuser'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$tid = $database->result($res, $i, "id");

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
$res = $database->exec("SELECT * FROM teammembers WHERE handle = '$ggzuser'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$teamname = $database->result($res, $i, "teamname");
	$role = $database->result($res, $i, "role");

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
rankings_players($lookup);
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
rankings_tournaments($lookup);
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
rankings_matches($lookup);
?>
	</div>
