<?php include("top.inc"); ?>
<?php include("database.php"); ?>
<?php include("stats.php"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Database: Players
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<?php stats_players($id, $lookup); ?>
	</div>
	<div class="text">
	Tournaments:
	<br>
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
	<div class="text">
	Teams:
	<br>
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
	<div class="text">
<?php
include("rankings.php");
rankings_players($id, $lookup);
?>
	</div>
</div>

<?php include("bottom.inc"); ?>
