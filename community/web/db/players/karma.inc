<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("player.php");
$player = new Player($lookup);

?>
	<h2>
		<span class="itemleader">:: </span>
		<?php $player->icon(); ?> Karma for player <?php echo $lookup; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php

include_once("auth.php");
$ggzuser = $lookup;
$res = $database->exec("SELECT * FROM karma WHERE fromhandle = '%^'", array($ggzuser));
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
$res = $database->exec("SELECT * FROM karma WHERE tohandle = '%^'", array($ggzuser));
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
