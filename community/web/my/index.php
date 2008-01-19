<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
include("top.inc");

include_once("player.php");

$ggzuser = Auth::username();

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community)
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	This is your personal page, which is only visible by you.
	Other players refer to you by your
	<a href="/db/players/?lookup=<?php echo $ggzuser; ?>">public page</a>.
	To modify the settings about you, please proceed to the
	<a href="change.php">change page</a>.
	</div>
	<div class="text">

<?php

$player = new Player($ggzuser);
$player->display();

?>

	</div>
<?php
if (Config::feature("datarepo")) :
$player->items(true);
endif;
?>

</div>

<?php include("bottom.inc"); ?>
