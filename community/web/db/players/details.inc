	<h2>
		<span class="itemleader">:: </span>
		Permission details for player <?php echo $lookup; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php

include_once("player.php");

$player = new Player($lookup);
$player->permissions();

?>
	</div>
