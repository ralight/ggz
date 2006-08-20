<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("player.php");
$player = new Player($lookup);

?>
	<h2>
		<span class="itemleader">:: </span>
		<?php $player->icon(); ?> Winner titles for player <?php echo $lookup; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
include("rankings.php");
rankings_matches($lookup);
?>
	</div>
