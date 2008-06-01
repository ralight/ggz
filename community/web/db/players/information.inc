<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("player.php");
$player = new Player($lookup);

if (!$player->registered) :
	echo "<div id='main'>";
	echo "<div class='text'>";
	echo __("No such player record was found in the database.");
	echo "</div>";
	echo "</div>";

	echo "</div>"; // from index.php
	include("bottom.inc");
	exit;
endif;
?>
	<h2>
		<span class="itemleader">:: </span>
		<?php $player->icon(); ?> Public information for player <?php echo html($lookup); ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
$player->display();
?>
	</div>
<?php if (Config::feature("karma")) : ?>
	<h2>
		<span class="itemleader">:: </span>
		Player Karma
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
if ($overall_ratings == 0) :
	$overall_ratings = 1;
endif;
$karma = 10 * (1 - ($bad_ratings / $overall_ratings));
if ($karma >= 8) :
	$karmapic = "karma_good";
	$karmatitle = "Good karma";
	$karmameaning = "This makes " . html($lookup) . " a most-wanted opponent and team mate.";
elseif ($karma >= 4) :
	$karmapic = "karma_medium";
	$karmatitle = "Medium karma";
	$karmameaning = "Feel free to play against " . html($lookup) . " and don't forget to give out karma points.";
else :
	$karmapic = "karma_bad";
	$karmatitle = "Bad karma";
	$karmameaning = "It's probably a good idea not wanting to play against " . html($lookup) . ".";
endif;
if ($player->registered) :
	echo "<img src='/db/ggzicons/misc/$karmapic.png' title='$karmatitle'>&nbsp;\n";
	echo "The player's overall karma score is $karma out of 10.<br>\n";
	echo $karmameaning;
	echo "The <a href='karma.php?lookup=" . html($lookup) . "'>karma page</a>\n";
	echo "contains the details.\n";
else :
	echo "Player not found.<br>\n";
endif;
?>
	</div>
<?php endif; ?>
	<div class="text">

<?php
$ggzuser = $lookup;
$res = $database->exec("SELECT * FROM users WHERE handle = '%^'", array($ggzuser));
if ($database->numrows($res) == 1) :
	$firstlogin = $database->result($res, 0, "firstlogin");
	if ($firstlogin > 0) :
		$veteran = (int)((time() - $firstlogin) / 86400);
	else :
		$veteran = 0;
	endif;

	if ($veteran > 730):
		$veteranpic = "veteran-level5";
		$veterantitle = "Veteran with honours";
		$veteranmeaning = "Only the best ever get thus far. A member of the council of elders.";
	elseif ($veteran > 365) :
		$veteranpic = "veteran-level4";
		$veterantitle = "Veteran";
		$veteranmeaning = "Candidate for the council of elders and overall long-time player.";
	elseif ($veteran > 180) :
		$veteranpic = "veteran-level3";
		$veterantitle = "Long-time player";
		$veteranmeaning = "The player has been here for many months already.";
	elseif ($veteran > 60) :
		$veteranpic = "veteran-level2";
		$veterantitle = "Known player";
		$veteranmeaning = "Plenty of time for the player to try out some games.";
	elseif ($veteran > 30) :
		$veteranpic = "veteran-level1";
		$veterantitle = "Amateur";
		$veteranmeaning = "The player has been around for a couple of days.";
	else :
		$veteranpic = "veteran-level0";
		$veterantitle = "Newbie";
		$veteranmeaning = "The player hasn't even known this site for a month or so.";
	endif;

	echo "<img src='/db/ggzicons/misc/$veteranpic.png' title='$veterantitle'>&nbsp;\n";
	echo "Registered member for $veteran days. This makes the player a $veterantitle!<br>\n";
	echo $veteranmeaning;
else :
	echo "Oops, user not found.<br>\n";
endif;
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
$res = $database->exec("SELECT * FROM tournaments WHERE organizer = '%^'", array($ggzuser));
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$tid = $database->result($res, $i, "id");

	echo "<a href='/db/tournaments/?lookup=$tid'>$name</a><br>\n";
}
if ($i == 0) :
	echo "The player hasn't participated in any tournament so far.";
endif;
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
$res = $database->exec("SELECT * FROM teammembers WHERE handle = '%^'", array($ggzuser));
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$teamname = $database->result($res, $i, "teamname");
	$role = $database->result($res, $i, "role");

	echo "<a href='/db/teams/?lookup=$teamname'>$teamname</a> ($role)<br>\n";
}
if ($i == 0) :
	echo "The player is not a member of any team.";
endif;
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
rankings_matches_summary($lookup);
?>
	</div>
