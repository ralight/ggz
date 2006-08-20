<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Tournaments
		<span class="itemleader"> :: </span>
		<a name="tournaments"></a>
	</h1>
	<div class="text">
	The tournament pages guide players to existing
	tournaments or to initiate new ones.
	</div>
	<?php include("tournaments.php"); ?>
</div>

<?php include("bottom.inc"); ?>
