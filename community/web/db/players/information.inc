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
		Player Karma
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include_once("auth.php");
$ggzuser = $lookup;
//$res = $database->exec("SELECT id FROM users WHERE handle = '$lookup'");
//if ($database->numrows($res) == 1) :
//	$playerid = database->result($res, 0, "id");
//endif;
$res = $database->exec("SELECT * FROM karma WHERE fromhandle = '$ggzuser'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "tohandle");
	$karma = $database->result($res, $i, "karma");

	echo "<a href='/db/players/?lookup=$name'>$name</a> received $karma points from $ggzuser.<br>\n";
}
if ($database->numrows($res) == 0) :
	echo "The player hasn't donated any karma points yet.<br>\n";
endif;
?>
	</div>
	<div class="text">
<?php
$res = $database->exec("SELECT * FROM karma WHERE tohandle = '$ggzuser'");
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "fromhandle");
	$karma = $database->result($res, $i, "karma");

	if (!$karma) :
		$bad_ratings += 1;
	endif;
	$overall_ratings += 1;

	echo "<a href='/db/players/?lookup=$name'>$name</a> gave $karma points to $ggzuser.<br>\n";
}
if ($database->numrows($res) == 0) :
	echo "The player hasn't received any karma points yet.<br>\n";
endif;
?>
	</div>
	<div class="text">
<?php
$karma = 10 * (1 - ($bad_ratings / $overall_ratings));
if ($karma >= 8) :
	$karmapic = "karma_good";
	$karmatitle = "Good karma";
elseif ($karma >= 4) :
	$karmapic = "karma_medium";
	$karmatitle = "Medium karma";
else :
	$karmapic = "karma_bad";
	$karmatitle = "Bad karma";
endif;
echo "<img src='/db/ggzicons/misc/$karmapic.png' title='$karmatitle'>&nbsp;\n";
echo "The player's overall karma score is $karma out of 10.<br>\n";
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
//include_once("auth.php");
//$ggzuser = $lookup;
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
