<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_rightbar = "disabled";
include("top.inc");
include("live.php");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community: Running Games
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<?php $live->output(); ?>
	</div>
</div>

<?php include("bottom.inc"); ?>
